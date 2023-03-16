#ifdef _WIN32
#include "WinAudio.h"
#include "StopWatch.h"
#include "ConsoleLogger.h"
#include <AudioProcessor.h>
#include <VersionHelpers.h>

using namespace Microsoft::WRL;

#define WINAUDIO_EXCPT(hr, winAudio, method, message) hres = hr; if(FAILED(hres)) { RAISE_AUDIO_EXCPT(winAudio, AudioException(hres, method, message)); throw AudioException(hres, method, message); }
#define WINAUDIO_RENDER_THREAD_EXCPT(hr, winAudio, method, message) hres = hr; if(FAILED(hres)) { RAISE_AUDIO_EXCPT(winAudio, AudioException(hres, method, message)); goto RENDER_EXIT; }
#define WINAUDIO_CAPTURE_THREAD_EXCPT(hr, winAudio, method, message) hres = hr; if(FAILED(hres)) { RAISE_AUDIO_EXCPT(winAudio, AudioException(hres, method, message)); goto CAPTURE_EXIT; }

namespace HephAudio
{
	namespace Native
	{
		WinAudio::WinAudio() : NativeAudio(), hEvent(nullptr), pRenderAudioClient(nullptr), pRenderSessionManager(nullptr), pRenderSessionControl(nullptr), pCaptureAudioClient(nullptr)
		{
			if (!IsWindowsVistaOrGreater())
			{
				throw AudioException(E_NOINTERFACE, "WinAudio", "OS version must be at least Windows Vista.");
			}

			CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			HRESULT hres;
			WINAUDIO_EXCPT(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &pEnumerator), this, "WinAudio", "An error occurred whilst initializing the audio device enumerator.");

			this->EnumerateAudioDevices();
			this->deviceThread = std::thread(&WinAudio::CheckAudioDevices, this);
		}
		WinAudio::~WinAudio()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing WinAudio...", ConsoleLogger::info);

			disposing = true;

			JoinDeviceThread();
			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();

			pEnumerator = nullptr;
			pCaptureAudioClient = nullptr;
			pRenderSessionControl = nullptr;
			pRenderSessionManager = nullptr;
			pRenderAudioClient = nullptr;

			CoUninitialize();

			HEPHAUDIO_LOG("WinAudio destructed in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
		}
		void WinAudio::SetMasterVolume(hephaudio_float volume)
		{
			if (pRenderSessionManager != nullptr)
			{
				if (volume > 1.0) { volume = 1.0; }
				if (volume < 0.0) { volume = 0.0; }

				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				HRESULT hres;
				WINAUDIO_EXCPT(pRenderSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), this, "WinAudio::SetMasterVolume", "An error occurred whilst setting the master volume.");

				WINAUDIO_EXCPT(pVolume->SetMasterVolume(volume, nullptr), this, "WinAudio::SetMasterVolume", "An error occurred whilst setting the master volume.");
			}
		}
		hephaudio_float WinAudio::GetMasterVolume() const
		{
			float volume = -1.0f;

			if (pRenderSessionManager != nullptr)
			{
				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				HRESULT hres;
				WINAUDIO_EXCPT(pRenderSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), this, "WinAudio::GetMasterVolume", "An error occurred whilst getting the master volume.");

				WINAUDIO_EXCPT(pVolume->GetMasterVolume(&volume), this, "WinAudio::GetMasterVolume", "An error occurred whilst getting the master volume.");
			}

			return volume;
		}
		void WinAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), ConsoleLogger::info);

			StopRendering();

			ComPtr<IMMDevice> pDevice = nullptr;
			WAVEFORMATEX* closestFormat = nullptr;
			HRESULT hres;

			if (device == nullptr || device->type != AudioDeviceType::Render || device->isDefault)
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice), this, "WinAudio::InitializeRender", "An error occurred whilst getting the render device.");
			}
			else
			{
				WINAUDIO_EXCPT(pEnumerator->GetDevice(device->id, &pDevice), this, "WinAudio::InitializeRender", "An error occurred whilst getting the render device.");
			}

			LPWSTR deviceId = nullptr;
			WINAUDIO_EXCPT(pDevice->GetId(&deviceId), this, "WinAudio::InitializeRender", "An error occurred whilst getting the render device.");
			if (deviceId != nullptr)
			{
				renderDeviceId = deviceId;
				CoTaskMemFree(deviceId);
			}
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, nullptr, (void**)pRenderAudioClient.GetAddressOf()), this, "WinAudio::InitializeRender", "An error occurred whilst activating the render device.");

			WAVEFORMATEX wrf = format;
			WINAUDIO_EXCPT(pRenderAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wrf, &closestFormat), this, "WinAudio::InitializeRender", "An error occurred whilst checking if the given format is supported.");
			if (closestFormat != nullptr)
			{
				format = (*closestFormat);
				format.formatTag = 1;
				format.headerSize = 0;
				wrf = format;
				CoTaskMemFree(closestFormat);
			}
			renderFormat = format;

			WINAUDIO_EXCPT(pRenderAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, &wrf, nullptr), this, "WinAudio::InitializeRender", "An error occurred whilst initializing the audio client.");
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, (void**)pRenderSessionManager.GetAddressOf()), this, "WinAudio::InitializeRender", "An error occurred whilst activating the session manager.");

			WINAUDIO_EXCPT(pRenderSessionManager->GetAudioSessionControl(nullptr, 0, pRenderSessionControl.GetAddressOf()), this, "WinAudio::InitializeRender", "An error occurred whilst getting the session controls.");

			hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			WINAUDIO_EXCPT(pRenderAudioClient->SetEventHandle(hEvent), this, "WinAudio::InitializeRender", "An error occurred whilst setting the audio client event handle.");

			isRenderInitialized = true;
			renderThread = std::thread(&WinAudio::RenderData, this);

			HEPHAUDIO_LOG("Render initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
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
				HEPHAUDIO_LOG("Stopped rendering.", ConsoleLogger::info);
			}
		}
		void WinAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), ConsoleLogger::info);

			StopCapturing();

			ComPtr<IMMDevice> pDevice = nullptr;
			WAVEFORMATEX* closestFormat = nullptr;
			HRESULT hres;

			if (device == nullptr || device->type != AudioDeviceType::Capture || device->isDefault)
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice), this, "WinAudio::InitializeCapture", "An error occurred whilst getting the device.");
			}
			else
			{
				WINAUDIO_EXCPT(pEnumerator->GetDevice(device->id, &pDevice), this, "WinAudio::InitializeCapture", "An error occurred whilst getting the device.");
			}

			LPWSTR deviceId = nullptr;
			WINAUDIO_EXCPT(pDevice->GetId(&deviceId), this, "WinAudio::InitializeCapture", "An error occurred whilst getting the device.");
			if (deviceId != nullptr)
			{
				captureDeviceId = deviceId;
				CoTaskMemFree(deviceId);
			}
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, nullptr, (void**)pCaptureAudioClient.GetAddressOf()), this, "WinAudio::InitializeCapture", "An error occurred whilst activating the device.");

			WAVEFORMATEX wcf = format;
			WINAUDIO_EXCPT(pCaptureAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wcf, &closestFormat), this, "WinAudio::InitializeCapture", "An error occurred whilst checking if the given format is supported.");
			if (closestFormat != nullptr)
			{
				format = (*closestFormat);
				format.formatTag = 1;
				format.headerSize = 0;
				wcf = format;
				CoTaskMemFree(closestFormat);
			}
			captureFormat = format;

			WINAUDIO_EXCPT(pCaptureAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, &wcf, nullptr), this, "WinAudio::InitializeCapture", "An error occurred whilst initializing the audio client.");

			isCaptureInitialized = true;
			captureThread = std::thread(&WinAudio::CaptureData, this);

			HEPHAUDIO_LOG("Capture initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
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
				HEPHAUDIO_LOG("Stopped capturing.", ConsoleLogger::info);
			}
		}
		void WinAudio::SetDisplayName(StringBuffer displayName)
		{
			if (pRenderSessionControl != nullptr)
			{
				this->displayName = displayName;
				HRESULT hres;
				WINAUDIO_EXCPT(pRenderSessionControl->SetDisplayName(displayName.fwc_str(), nullptr), this, "WinAudio::SetDisplayName", "An error occurred whilst setting the display name.");
			}
		}
		void WinAudio::SetIconPath(StringBuffer iconPath)
		{
			if (pRenderSessionControl != nullptr)
			{
				this->iconPath = iconPath;
				HRESULT hres;
				WINAUDIO_EXCPT(pRenderSessionControl->SetIconPath(iconPath.fwc_str(), nullptr), this, "WinAudio::SetIconPath", "An error occurred whilst setting the icon path.");
			}
		}
		void WinAudio::EnumerateAudioDevices()
		{
			if (pEnumerator != nullptr)
			{
				const EDataFlow dataFlow = EDataFlow::eAll;
				ComPtr<IMMDeviceCollection> pCollection = nullptr;
				ComPtr<IMMDevice> pDefaultRender = nullptr;
				ComPtr<IMMDevice> pDefaultCapture = nullptr;
				LPWSTR defaultRenderId = nullptr;
				LPWSTR defaultCaptureId = nullptr;
				HRESULT hres;

				this->audioDevices.clear();

				WINAUDIO_EXCPT(pEnumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &pCollection), this, "WinAudio::GetAudioDevices", "An error occurred whilst enumerating the devices.");

				// Get default render devices id, we will use it later to identify the default render device.
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultRender), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default render device id.");
				WINAUDIO_EXCPT(pDefaultRender->GetId(&defaultRenderId), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default render device id.");
				StringBuffer defaultRenderIdStr = defaultRenderId != nullptr ? defaultRenderId : L"";

				// Get default capture devices id, we will use it later to identify the default capture device.
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultCapture), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default capture device id.");
				WINAUDIO_EXCPT(pDefaultCapture->GetId(&defaultCaptureId), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default capture device id.");
				StringBuffer defaultCaptureIdStr = defaultCaptureId != nullptr ? defaultCaptureId : L"";

				UINT deviceCount = 0;
				WINAUDIO_EXCPT(pCollection->GetCount(&deviceCount), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the device count.");

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
					WINAUDIO_EXCPT(pCollection->Item(i, &pDevice), this, "WinAudio::GetAudioDevices", "An error occurred whilst reading the devices properties.");
					WINAUDIO_EXCPT(pDevice->GetId(&deviceId), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the device id.");

					// Get device data flow.
					WINAUDIO_EXCPT(pDevice->QueryInterface(__uuidof(IMMEndpoint), &pEndpoint), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the device type.");
					WINAUDIO_EXCPT(pEndpoint->GetDataFlow(&dataFlow), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the device type.");

					// Open the property store of the device to read device info.
					WINAUDIO_EXCPT(pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore), this, "WinAudio::GetAudioDevices", "An error occurred whilst reading the devices properties.");
					WINAUDIO_EXCPT(pPropertyStore->GetValue(PKEY_Device_FriendlyName, &variant), this, "WinAudio::GetAudioDevices", "An error occurred whilst reading the devices properties.");

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
			}
		}
		void WinAudio::RenderData()
		{
			ComPtr<IAudioRenderClient> pRenderClient = nullptr;
			AudioBuffer dataBuffer;
			UINT32 padding, nFramesAvailable;
			BYTE* renderBuffer;
			UINT32 bufferSize;
			HRESULT hres;

			WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->GetBufferSize(&bufferSize), this, "WinAudio", "An error occurred whilst rendering the samples.");
			dataBuffer = AudioBuffer(bufferSize, renderFormat);

			WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)pRenderClient.GetAddressOf()), this, "WinAudio", "An error occurred whilst rendering the samples.");
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(bufferSize, &renderBuffer), this, "WinAudio", "An error occurred whilst rendering the samples.");
			memset(renderBuffer, 0, bufferSize * renderFormat.FrameSize());
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(bufferSize, 0), this, "WinAudio", "An error occurred whilst rendering the samples.");

			WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->Start(), this, "WinAudio", "An error occurred whilst rendering the samples.");

			while (!disposing && isRenderInitialized)
			{
				uint32_t retval = WaitForSingleObject(hEvent, 2000);
				if (retval != WAIT_OBJECT_0)
				{
					WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->Stop(), this, "WinAudio", "An error occurred whilst rendering the samples.");
					WINAUDIO_RENDER_THREAD_EXCPT(E_FAIL, this, "WinAudio", "An error occurred whilst rendering the samples.");
				}

				WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->GetCurrentPadding(&padding), this, "WinAudio", "An error occurred whilst rendering the samples.");
				nFramesAvailable = bufferSize - padding;

				if (nFramesAvailable > 0)
				{
					Mix(dataBuffer, nFramesAvailable);

					WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(nFramesAvailable, &renderBuffer), this, "WinAudio", "An error occurred whilst rendering the samples.");
					memcpy(renderBuffer, dataBuffer.Begin(), nFramesAvailable * renderFormat.FrameSize());
					WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(nFramesAvailable, 0), this, "WinAudio", "An error occurred whilst rendering the samples.");

					dataBuffer.Reset();
				}
			}

			WINAUDIO_RENDER_THREAD_EXCPT(pRenderAudioClient->Stop(), this, "WinAudio", "An error occurred whilst rendering the samples.");

		RENDER_EXIT:
			if (hEvent != nullptr)
			{
				CloseHandle(hEvent);
				hEvent = nullptr;
			}
		}
		void WinAudio::CaptureData()
		{
			constexpr REFERENCE_TIME reftimesPerSec = 1e6;
			constexpr REFERENCE_TIME reftimesPerMilliSec = 2e3;

			ComPtr<IAudioCaptureClient> pCaptureClient = nullptr;
			UINT32 bufferSize, nFramesAvailable, packetLength;
			BYTE* captureBuffer = nullptr;
			DWORD flags = 0;
			REFERENCE_TIME hnsActualDuration = 0;
			HRESULT hres;

			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->GetBufferSize(&bufferSize), this, "WinAudio", "An error occurred whilst capturing the samples.");
			hnsActualDuration = (hephaudio_float)reftimesPerSec * bufferSize / captureFormat.sampleRate;

			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->GetService(__uuidof(IAudioCaptureClient), &pCaptureClient), this, "WinAudio", "An error occurred whilst capturing the samples.");
			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->Start(), this, "WinAudio", "An error occurred whilst capturing the samples.");

			while (!disposing && isCaptureInitialized)
			{
				if (!isCapturePaused && OnCapture)
				{
					Sleep(hnsActualDuration / reftimesPerMilliSec);

					WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetNextPacketSize(&packetLength), this, "WinAudio", "An error occurred whilst capturing the samples.");
					while (packetLength != 0)
					{
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetBuffer(&captureBuffer, &nFramesAvailable, &flags, nullptr, nullptr), this, "WinAudio", "An error occurred whilst capturing the samples.");

						AudioBuffer temp(nFramesAvailable, captureFormat);
						memcpy(temp.Begin(), captureBuffer, temp.Size());
						AudioProcessor::ConvertPcmToInnerFormat(temp);
						AudioCaptureEventArgs captureEventArgs = AudioCaptureEventArgs(this, temp);
						OnCapture(&captureEventArgs, nullptr);

						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->ReleaseBuffer(nFramesAvailable), this, "WinAudio", "An error occurred whilst capturing the samples.");
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetNextPacketSize(&packetLength), this, "WinAudio", "An error occurred whilst capturing the samples.");
					}
				}
			}

			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->Stop(), this, "WinAudio", "An error occurred whilst capturing the samples.");

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
	}
}
#endif