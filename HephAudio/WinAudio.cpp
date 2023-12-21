#ifdef _WIN32
#include "WinAudio.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "../HephCommon/HeaderFiles/StopWatch.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"
#include "AudioProcessor.h"
#include <VersionHelpers.h>

using namespace Microsoft::WRL;

#define WINAUDIO_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(hres, method, message)); }
#define WINAUDIO_RENDER_THREAD_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(hres, method, message)); goto RENDER_EXIT; }
#define WINAUDIO_CAPTURE_THREAD_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(hres, method, message)); goto CAPTURE_EXIT; }
#define WINAUDIO_ENUMERATE_DEVICE_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(hres, method, message)); return NativeAudio::DEVICE_ENUMERATION_FAIL; }

namespace HephAudio
{
	namespace Native
	{
		WinAudio::WinAudio() : NativeAudio(), hEvent(nullptr), pRenderAudioClient(nullptr), pRenderSessionManager(nullptr), pRenderSessionControl(nullptr), pCaptureAudioClient(nullptr)
		{
			if (!IsWindowsVistaOrGreater())
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(E_NOINTERFACE, "WinAudio", "OS version must be at least Windows Vista."));
			}

			HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY | COINIT_DISABLE_OLE1DDE);
			if (FAILED(hres))
			{
				if (hres == RPC_E_CHANGED_MODE)
				{
					WINAUDIO_EXCPT(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY | COINIT_DISABLE_OLE1DDE), "WinAudio", "An error occurred whilst initializing COM.");
				}
				else
				{
					WINAUDIO_EXCPT(hres, "WinAudio", "An error occurred whilst initializing COM.");
				}
			}

			WINAUDIO_EXCPT(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &pEnumerator), "WinAudio", "An error occurred whilst initializing the audio device enumerator.");

			this->EnumerateAudioDevices();
			this->deviceThread = std::thread(&WinAudio::CheckAudioDevices, this);
		}
		WinAudio::~WinAudio()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing WinAudio...", HEPH_CL_INFO);

			disposing = true;

			JoinDeviceThread();
			JoinRenderThread();
			JoinCaptureThread();

			pEnumerator = nullptr;
			pCaptureAudioClient = nullptr;
			pRenderSessionControl = nullptr;
			pRenderSessionManager = nullptr;
			pRenderAudioClient = nullptr;

			CoUninitialize();

			HEPHAUDIO_LOG("WinAudio destructed in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudio::SetMasterVolume(heph_float volume)
		{
			if (pRenderSessionManager != nullptr)
			{
				if (volume > 1.0) { volume = 1.0; }
				if (volume < 0.0) { volume = 0.0; }

				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				HRESULT hres;
				WINAUDIO_EXCPT(pRenderSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), "WinAudio::SetMasterVolume", "An error occurred whilst setting the master volume.");

				WINAUDIO_EXCPT(pVolume->SetMasterVolume(volume, nullptr), "WinAudio::SetMasterVolume", "An error occurred whilst setting the master volume.");
			}
		}
		heph_float WinAudio::GetMasterVolume() const
		{
			float volume = -1.0f;

			if (pRenderSessionManager != nullptr)
			{
				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				HRESULT hres;
				WINAUDIO_EXCPT(pRenderSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), "WinAudio::GetMasterVolume", "An error occurred whilst getting the master volume.");

				WINAUDIO_EXCPT(pVolume->GetMasterVolume(&volume), "WinAudio::GetMasterVolume", "An error occurred whilst getting the master volume.");
			}

			return volume;
		}
		void WinAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			StopRendering();

			ComPtr<IMMDevice> pDevice = nullptr;
			WAVEFORMATEX* closestFormat = nullptr;
			HRESULT hres;

			if (device == nullptr || device->type != AudioDeviceType::Render || device->isDefault)
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice), "WinAudio::InitializeRender", "An error occurred whilst getting the render device.");
			}
			else
			{
				WINAUDIO_EXCPT(pEnumerator->GetDevice(device->id.wc_str(), &pDevice), "WinAudio::InitializeRender", "An error occurred whilst getting the render device.");
			}

			LPWSTR deviceId = nullptr;
			WINAUDIO_EXCPT(pDevice->GetId(&deviceId), "WinAudio::InitializeRender", "An error occurred whilst getting the render device.");
			if (deviceId != nullptr)
			{
				renderDeviceId = deviceId;
				CoTaskMemFree(deviceId);
			}
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)pRenderAudioClient.GetAddressOf()), "WinAudio::InitializeRender", "An error occurred whilst activating the render device.");

			WAVEFORMATEX wrf = AFI2WFX(format);
			WINAUDIO_EXCPT(pRenderAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wrf, &closestFormat), "WinAudio::InitializeRender", "An error occurred whilst checking if the given format is supported.");
			if (closestFormat != nullptr)
			{
				format = WFX2AFI(*closestFormat);
				format.formatTag = WAVE_FORMAT_PCM;
				wrf = AFI2WFX(format);
				CoTaskMemFree(closestFormat);
			}
			renderFormat = format;

			WINAUDIO_EXCPT(pRenderAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, &wrf, nullptr), "WinAudio::InitializeRender", "An error occurred whilst initializing the audio client.");
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioSessionManager), CLSCTX_INPROC_SERVER, nullptr, (void**)pRenderSessionManager.GetAddressOf()), "WinAudio::InitializeRender", "An error occurred whilst activating the session manager.");

			WINAUDIO_EXCPT(pRenderSessionManager->GetAudioSessionControl(nullptr, 0, pRenderSessionControl.GetAddressOf()), "WinAudio::InitializeRender", "An error occurred whilst getting the session controls.");

			hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			WINAUDIO_EXCPT(pRenderAudioClient->SetEventHandle(hEvent), "WinAudio::InitializeRender", "An error occurred whilst setting the audio client event handle.");

			isRenderInitialized = true;
			renderThread = std::thread(&WinAudio::RenderData, this);

			HEPHAUDIO_LOG("Render initialized in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudio::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = L"";
				JoinRenderThread();
				HRESULT hres;
				pRenderSessionControl = nullptr;
				pRenderSessionManager = nullptr;
				pRenderAudioClient = nullptr;
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void WinAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			StopCapturing();

			ComPtr<IMMDevice> pDevice = nullptr;
			WAVEFORMATEX* closestFormat = nullptr;
			HRESULT hres;

			if (device == nullptr || device->type != AudioDeviceType::Capture || device->isDefault)
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice), "WinAudio::InitializeCapture", "An error occurred whilst getting the device.");
			}
			else
			{
				WINAUDIO_EXCPT(pEnumerator->GetDevice(device->id.wc_str(), &pDevice), "WinAudio::InitializeCapture", "An error occurred whilst getting the device.");
			}

			LPWSTR deviceId = nullptr;
			WINAUDIO_EXCPT(pDevice->GetId(&deviceId), "WinAudio::InitializeCapture", "An error occurred whilst getting the device.");
			if (deviceId != nullptr)
			{
				captureDeviceId = deviceId;
				CoTaskMemFree(deviceId);
			}
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)pCaptureAudioClient.GetAddressOf()), "WinAudio::InitializeCapture", "An error occurred whilst activating the device.");

			WAVEFORMATEX wcf = AFI2WFX(format);
			WINAUDIO_EXCPT(pCaptureAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wcf, &closestFormat), "WinAudio::InitializeCapture", "An error occurred whilst checking if the given format is supported.");
			if (closestFormat != nullptr)
			{
				format = WFX2AFI(*closestFormat);
				format.formatTag = WAVE_FORMAT_PCM;
				wcf = AFI2WFX(format);
				CoTaskMemFree(closestFormat);
			}
			captureFormat = format;

			WINAUDIO_EXCPT(pCaptureAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, &wcf, nullptr), "WinAudio::InitializeCapture", "An error occurred whilst initializing the audio client.");

			isCaptureInitialized = true;
			captureThread = std::thread(&WinAudio::CaptureData, this);

			HEPHAUDIO_LOG("Capture initialized in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudio::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false;
				captureDeviceId = L"";
				JoinCaptureThread();
				HRESULT hres;
				pCaptureAudioClient = nullptr;
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void WinAudio::SetDisplayName(HephCommon::StringBuffer displayName)
		{
			if (pRenderSessionControl != nullptr)
			{
				this->displayName = displayName;
				HRESULT hres;
				WINAUDIO_EXCPT(pRenderSessionControl->SetDisplayName(displayName.fwc_str(), nullptr), "WinAudio::SetDisplayName", "An error occurred whilst setting the display name.");
			}
		}
		void WinAudio::SetIconPath(HephCommon::StringBuffer iconPath)
		{
			if (pRenderSessionControl != nullptr)
			{
				this->iconPath = iconPath;
				HRESULT hres;
				WINAUDIO_EXCPT(pRenderSessionControl->SetIconPath(iconPath.fwc_str(), nullptr), "WinAudio::SetIconPath", "An error occurred whilst setting the icon path.");
			}
		}
		bool WinAudio::EnumerateAudioDevices()
		{
			const EDataFlow dataFlow = EDataFlow::eAll;
			ComPtr<IMMDeviceCollection> pCollection = nullptr;
			ComPtr<IMMDevice> pDefaultRender = nullptr;
			ComPtr<IMMDevice> pDefaultCapture = nullptr;
			LPWSTR defaultRenderId = nullptr;
			LPWSTR defaultCaptureId = nullptr;
			HRESULT hres;

			this->audioDevices.clear();

			WINAUDIO_ENUMERATE_DEVICE_EXCPT(pEnumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &pCollection), "WinAudio::GetAudioDevices", "An error occurred whilst enumerating the devices.");

			// Get default render devices id, we will use it later to identify the default render device.
			hres = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultRender);
			if (hres != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
			{
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(hres, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default render device id.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDefaultRender->GetId(&defaultRenderId), "WinAudio::GetAudioDevices", "An error occurred whilst getting the default render device id.");
			}
			HephCommon::StringBuffer defaultRenderIdStr = defaultRenderId != nullptr ? defaultRenderId : L"";

			// Get default capture devices id, we will use it later to identify the default capture device.
			hres = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultCapture);
			if (hres != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
			{
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(hres, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default capture device id.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDefaultCapture->GetId(&defaultCaptureId), "WinAudio::GetAudioDevices", "An error occurred whilst getting the default capture device id.");
			}
			HephCommon::StringBuffer defaultCaptureIdStr = defaultCaptureId != nullptr ? defaultCaptureId : L"";

			UINT deviceCount = 0;
			WINAUDIO_ENUMERATE_DEVICE_EXCPT(pCollection->GetCount(&deviceCount), "WinAudio::GetAudioDevices", "An error occurred whilst getting the device count.");

			for (UINT i = 0; i < deviceCount; i++)
			{
				ComPtr<IMMDevice> pDevice = nullptr;
				ComPtr<IPropertyStore> pPropertyStore = nullptr;
				ComPtr<IMMEndpoint> pEndpoint = nullptr;
				AudioDevice device;
				LPWSTR deviceId;
				EDataFlow dataFlow;
				PROPVARIANT variant;

				PropVariantInit(&variant);
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pCollection->Item(i, &pDevice), "WinAudio::GetAudioDevices", "An error occurred whilst reading the devices properties.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDevice->GetId(&deviceId), "WinAudio::GetAudioDevices", "An error occurred whilst getting the device id.");

				// Get device data flow.
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDevice->QueryInterface(__uuidof(IMMEndpoint), &pEndpoint), "WinAudio::GetAudioDevices", "An error occurred whilst getting the device type.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pEndpoint->GetDataFlow(&dataFlow), "WinAudio::GetAudioDevices", "An error occurred whilst getting the device type.");

				// Open the property store of the device to read device info.
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore), "WinAudio::GetAudioDevices", "An error occurred whilst reading the devices properties.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pPropertyStore->GetValue(PKEY_Device_FriendlyName, &variant), "WinAudio::GetAudioDevices", "An error occurred whilst reading the devices properties.");

				if (variant.vt != VT_EMPTY)
				{
					device.name = variant.pwszVal;
					PropVariantClear(&variant);
				}

				device.id = deviceId;
				device.type = DataFlowToDeviceType(dataFlow);
				device.isDefault = (device.type == AudioDeviceType::Render && device.id == defaultRenderIdStr) || (device.type == AudioDeviceType::Capture && device.id == defaultCaptureIdStr);
				this->audioDevices.push_back(device);

				if (deviceId != nullptr)
				{
					CoTaskMemFree(deviceId);
				}
			}
			if (defaultRenderId != nullptr)
			{
				CoTaskMemFree(defaultRenderId);
			}

			if (defaultCaptureId != nullptr)
			{
				CoTaskMemFree(defaultCaptureId);
			}

			return NativeAudio::DEVICE_ENUMERATION_SUCCESS;
		}
		void WinAudio::RenderData()
		{
			ComPtr<IAudioRenderClient> pRenderClient = nullptr;
			AudioBuffer dataBuffer;
			UINT32 padding, nFramesAvailable;
			BYTE* renderBuffer;
			UINT32 bufferSize;
			HRESULT hres;

			WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->GetBufferSize(&bufferSize), "WinAudio", "An error occurred whilst rendering the samples.");
			dataBuffer = AudioBuffer(bufferSize, renderFormat);

			WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)pRenderClient.GetAddressOf()), "WinAudio", "An error occurred whilst rendering the samples.");
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(bufferSize, &renderBuffer), "WinAudio", "An error occurred whilst rendering the samples.");
			memset(renderBuffer, 0, bufferSize * renderFormat.FrameSize());
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(bufferSize, 0), "WinAudio", "An error occurred whilst rendering the samples.");

			WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->Start(), "WinAudio", "An error occurred whilst rendering the samples.");

			while (!disposing && isRenderInitialized)
			{
				uint32_t retval = WaitForSingleObject(hEvent, 2000);
				if (retval != WAIT_OBJECT_0)
				{
					WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->Stop(), "WinAudio", "An error occurred whilst rendering the samples.");
					WINAUDIO_RENDER_THREAD_EXCPT(E_FAIL, "WinAudio", "An error occurred whilst rendering the samples.");
				}

				WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->GetCurrentPadding(&padding), "WinAudio", "An error occurred whilst rendering the samples.");
				nFramesAvailable = bufferSize - padding;

				if (nFramesAvailable > 0)
				{
					Mix(dataBuffer, nFramesAvailable);

					WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(nFramesAvailable, &renderBuffer), "WinAudio", "An error occurred whilst rendering the samples.");
					memcpy(renderBuffer, dataBuffer.Begin(), nFramesAvailable * renderFormat.FrameSize());
					WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(nFramesAvailable, 0), "WinAudio", "An error occurred whilst rendering the samples.");

					dataBuffer.Reset();
				}
			}

			WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->Stop(), "WinAudio", "An error occurred whilst rendering the samples.");

		RENDER_EXIT:
			if (hEvent != nullptr)
			{
				CloseHandle(hEvent);
				hEvent = nullptr;
			}
		}
		void WinAudio::CaptureData()
		{
			HEPH_CONSTEXPR REFERENCE_TIME reftimesPerSec = 1e6;
			HEPH_CONSTEXPR REFERENCE_TIME reftimesPerMilliSec = 2e3;

			ComPtr<IAudioCaptureClient> pCaptureClient = nullptr;
			UINT32 bufferSize, nFramesAvailable, packetLength;
			BYTE* captureBuffer = nullptr;
			DWORD flags = 0;
			REFERENCE_TIME hnsActualDuration = 0;
			HRESULT hres;

			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->GetBufferSize(&bufferSize), "WinAudio", "An error occurred whilst capturing the samples.");
			hnsActualDuration = (heph_float)reftimesPerSec * bufferSize / captureFormat.sampleRate;

			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->GetService(__uuidof(IAudioCaptureClient), &pCaptureClient), "WinAudio", "An error occurred whilst capturing the samples.");
			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->Start(), "WinAudio", "An error occurred whilst capturing the samples.");

			while (!disposing && isCaptureInitialized)
			{
				if (!isCapturePaused && OnCapture)
				{
					Sleep(hnsActualDuration / reftimesPerMilliSec);

					WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetNextPacketSize(&packetLength), "WinAudio", "An error occurred whilst capturing the samples.");
					while (packetLength != 0)
					{
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetBuffer(&captureBuffer, &nFramesAvailable, &flags, nullptr, nullptr), "WinAudio", "An error occurred whilst capturing the samples.");

						AudioBuffer temp(nFramesAvailable, captureFormat);
						memcpy(temp.Begin(), captureBuffer, temp.Size());
						AudioProcessor::ConvertToInnerFormat(temp);
						AudioCaptureEventArgs captureEventArgs = AudioCaptureEventArgs(this, temp);
						OnCapture(&captureEventArgs, nullptr);

						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->ReleaseBuffer(nFramesAvailable), "WinAudio", "An error occurred whilst capturing the samples.");
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetNextPacketSize(&packetLength), "WinAudio", "An error occurred whilst capturing the samples.");
					}
				}
			}

			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->Stop(), "WinAudio", "An error occurred whilst capturing the samples.");

		CAPTURE_EXIT:;
		}
		EDataFlow WinAudio::DeviceTypeToDataFlow(AudioDeviceType deviceType)
		{
			switch (deviceType)
			{
			case AudioDeviceType::Render:
				return EDataFlow::eRender;
			case AudioDeviceType::Capture:
				return EDataFlow::eCapture;
			case AudioDeviceType::All:
				return EDataFlow::eAll;
			default:
				break;
			}
			return EDataFlow::EDataFlow_enum_count;
		}
		AudioDeviceType WinAudio::DataFlowToDeviceType(EDataFlow dataFlow)
		{
			switch (dataFlow)
			{
			case EDataFlow::eRender:
				return AudioDeviceType::Render;
			case EDataFlow::eCapture:
				return AudioDeviceType::Capture;
			case EDataFlow::eAll:
				return AudioDeviceType::All;
			default:
				break;
			}
			return AudioDeviceType::Null;
		}
		AudioFormatInfo WinAudio::WFX2AFI(const WAVEFORMATEX& wfx) noexcept
		{
			return AudioFormatInfo(wfx.wFormatTag, wfx.nChannels, wfx.nSamplesPerSec, wfx.wBitsPerSample);
		}
		WAVEFORMATEX WinAudio::AFI2WFX(const AudioFormatInfo& afi) noexcept
		{
			WAVEFORMATEX wfx{ 0 };
			wfx.wFormatTag = afi.formatTag;
			wfx.nChannels = afi.channelCount;
			wfx.nSamplesPerSec = afi.sampleRate;
			wfx.nAvgBytesPerSec = afi.ByteRate();
			wfx.nBlockAlign = afi.FrameSize();
			wfx.wBitsPerSample = afi.bitsPerSample;
			wfx.cbSize = 0;
			return wfx;
		}
	}
}
#endif