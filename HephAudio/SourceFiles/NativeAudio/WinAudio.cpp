#ifdef _WIN32
#include "NativeAudio/WinAudio.h"
#include "Stopwatch.h"
#include "ConsoleLogger.h"
#include "Exceptions/ExternalException.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Exceptions/NotSupportedException.h"
#include "Exceptions/TimeoutException.h"
#include <VersionHelpers.h>
#include <functiondiscoverykeys_devpkey.h>
#include <audioclient.h>
#include <comdef.h>

using namespace Heph;
using namespace Microsoft::WRL;

#define PKEY_DEVICE_FRIENDLY_NAME {{ 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } }, 14}
#define WINAUDIO_EXCPT_RET_VOID(hr, method, message) hres = hr; if(FAILED(hres)) { HEPH_RAISE_EXCEPTION(this, ExternalException(method, message, "WASAPI", WinAudioBase::GetComErrorMessage(hres))); return; }
#define WINAUDIO_EXCPT(hr, method, message, retval) hres = hr; if(FAILED(hres)) { HEPH_RAISE_EXCEPTION(this, ExternalException(method, message, "WASAPI", WinAudioBase::GetComErrorMessage(hres))); return retval; }
#define WINAUDIO_RENDER_THREAD_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { HEPH_RAISE_EXCEPTION(this, ExternalException(method, message, "WASAPI", WinAudioBase::GetComErrorMessage(hres))); goto RENDER_EXIT; }
#define WINAUDIO_CAPTURE_THREAD_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { HEPH_RAISE_EXCEPTION(this, ExternalException(method, message, "WASAPI", WinAudioBase::GetComErrorMessage(hres))); goto CAPTURE_EXIT; }
#define WINAUDIO_ENUMERATE_DEVICE_EXCPT(hr, method, message) hres = hr; if(FAILED(hres)) { HEPH_RAISE_EXCEPTION(this, ExternalException(method, message, "WASAPI", WinAudioBase::GetComErrorMessage(hres))); return NativeAudio::DEVICE_ENUMERATION_FAIL; }
#define WINAUDIO_MS_TO_REF_TIME(ms) (ms * 1e4)

namespace HephAudio
{
	namespace Native
	{
		WinAudio::WinAudio() : WinAudioBase(), pEnumerator(nullptr)
		{
			if (!IsWindowsVistaOrGreater())
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, NotSupportedException(HEPH_FUNC, "OS version must be at least Windows Vista."));
			}

			this->InitializeCOM();
			this->deviceThread = std::thread(&WinAudio::CheckAudioDevices, this);

			size_t i = 0;
			while (this->pEnumerator == nullptr)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); // wait for device thread to create the pEnumerator
				i++;
				if (i == 20)
				{
					HEPH_RAISE_AND_THROW_EXCEPTION(this, TimeoutException(HEPH_FUNC, "Timeout while waiting for the creation of the device enumerator."));
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(this->deviceEnumerationPeriod_ms + 100)); // wait for the first device enumeration
		}
		WinAudio::~WinAudio()
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG("Destructing WinAudio...", HEPH_CL_INFO);

			this->disposing = true;

			this->JoinDeviceThread();
			this->JoinRenderThread();
			this->JoinCaptureThread();

			CoUninitialize();

			HEPHAUDIO_LOG("WinAudio destructed in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudio::SetMasterVolume(double volume)
		{
			if (this->pRenderSessionManager != nullptr)
			{
				if (volume > 1.0) { volume = 1.0; }
				else if (volume < 0.0) { volume = 0.0; }

				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				HRESULT hres;
				WINAUDIO_EXCPT_RET_VOID(this->pRenderSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), HEPH_FUNC, "An error occurred while setting the master volume.");
				WINAUDIO_EXCPT_RET_VOID(pVolume->SetMasterVolume(volume, nullptr), HEPH_FUNC, "An error occurred while setting the master volume.");
				pVolume = nullptr;
			}
		}
		double WinAudio::GetMasterVolume() const
		{
			float volume = -1.0f;

			if (this->pRenderSessionManager != nullptr)
			{
				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				HRESULT hres;
				WINAUDIO_EXCPT(this->pRenderSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), HEPH_FUNC, "An error occurred while getting the master volume.", volume);
				WINAUDIO_EXCPT(pVolume->GetMasterVolume(&volume), HEPH_FUNC, "An error occurred while getting the master volume.", volume);
				pVolume = nullptr;
			}

			return volume;
		}
		void WinAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : ("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			this->StopRendering();

			this->renderThread = std::thread(&WinAudio::RenderData, this, device, format);
			size_t i = 0;
			while (!this->isRenderInitialized && i < 20) // wait for initialization to complete
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				i++;
			}

			HEPHAUDIO_LOG("Render initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudio::StopRendering()
		{
			if (this->isRenderInitialized)
			{
				this->isRenderInitialized = false;
				this->renderDeviceId = "";
				this->JoinRenderThread();
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void WinAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPH_SW_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : ("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			this->StopCapturing();

			this->captureThread = std::thread(&WinAudio::CaptureData, this, device, format);
			size_t i = 0;
			while (!this->isCaptureInitialized && i < 20) // wait for initialization to complete
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				i++;
			}

			HEPHAUDIO_LOG("Capture initialized in " + StringHelpers::ToString(HEPH_SW_DT_MS, 4) + " ms.", HEPH_CL_INFO);
		}
		void WinAudio::StopCapturing()
		{
			if (this->isCaptureInitialized)
			{
				this->isCaptureInitialized = false;
				this->captureDeviceId = "";
				this->JoinCaptureThread();
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		void WinAudio::GetNativeParams(NativeAudioParams& nativeParams) const
		{
			WasapiParams* pWasapiParams = dynamic_cast<WasapiParams*>(&nativeParams);
			if (pWasapiParams == nullptr)
			{
				HEPH_RAISE_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "nativeParams must be a WasapiParams instance."));
				return;
			}
			(*pWasapiParams) = this->params;
		}
		void WinAudio::SetNativeParams(const NativeAudioParams& nativeParams)
		{
			const WasapiParams* pWasapiParams = dynamic_cast<const WasapiParams*>(&nativeParams);
			if (pWasapiParams == nullptr)
			{
				HEPH_RAISE_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "nativeParams must be a WasapiParams instance."));
				return;
			}
			this->params = *pWasapiParams;
		}
		void WinAudio::SetDisplayName(const std::string& displayName)
		{
			if (this->pRenderSessionControl != nullptr)
			{
				HRESULT hres;
				WINAUDIO_EXCPT_RET_VOID(this->pRenderSessionControl->SetDisplayName(StringHelpers::StrToWide(displayName.c_str()).c_str(), nullptr), HEPH_FUNC, "An error occurred while setting the display name.");
			}
		}
		void WinAudio::SetIconPath(const std::filesystem::path& iconPath)
		{
			if (this->pRenderSessionControl != nullptr)
			{
				HRESULT hres;
				WINAUDIO_EXCPT_RET_VOID(this->pRenderSessionControl->SetIconPath(iconPath.wstring().c_str(), nullptr), HEPH_FUNC, "An error occurred while setting the icon path.");
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

			WINAUDIO_ENUMERATE_DEVICE_EXCPT(this->pEnumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &pCollection), HEPH_FUNC, "An error occurred while enumerating the devices.");

			// Get default render devices id, we will use it later to identify the default render device.
			hres = this->pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultRender);
			if (hres != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
			{
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(hres, HEPH_FUNC, "An error occurred while getting the default render device id.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDefaultRender->GetId(&defaultRenderId), HEPH_FUNC, "An error occurred while getting the default render device id.");
			}
			std::string defaultRenderIdStr = StringHelpers::WideToStr(defaultRenderId != nullptr ? defaultRenderId : L"");

			// Get default capture devices id, we will use it later to identify the default capture device.
			hres = this->pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultCapture);
			if (hres != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
			{
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(hres, HEPH_FUNC, "An error occurred while getting the default capture device id.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDefaultCapture->GetId(&defaultCaptureId), HEPH_FUNC, "An error occurred while getting the default capture device id.");
			}
			std::string defaultCaptureIdStr = StringHelpers::WideToStr(defaultCaptureId != nullptr ? defaultCaptureId : L"");

			UINT deviceCount = 0;
			WINAUDIO_ENUMERATE_DEVICE_EXCPT(pCollection->GetCount(&deviceCount), HEPH_FUNC, "An error occurred while getting the device count.");

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
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pCollection->Item(i, &pDevice), HEPH_FUNC, "An error occurred while reading the devices properties.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDevice->GetId(&deviceId), HEPH_FUNC, "An error occurred while getting the device id.");

				// Get device data flow.
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDevice->QueryInterface(__uuidof(IMMEndpoint), &pEndpoint), HEPH_FUNC, "An error occurred while getting the device type.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pEndpoint->GetDataFlow(&dataFlow), HEPH_FUNC, "An error occurred while getting the device type.");

				// Open the property store of the device to read device info.
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore), HEPH_FUNC, "An error occurred while reading the devices properties.");
				WINAUDIO_ENUMERATE_DEVICE_EXCPT(pPropertyStore->GetValue(PKEY_DEVICE_FRIENDLY_NAME, &variant), HEPH_FUNC, "An error occurred while reading the devices properties.");

				if (variant.vt != VT_EMPTY)
				{
					device.name = StringHelpers::WideToStr(variant.pwszVal);
					PropVariantClear(&variant);
				}

				device.id = StringHelpers::WideToStr(deviceId);
				device.type = WinAudio::DataFlowToDeviceType(dataFlow);
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
		void WinAudio::CheckAudioDevices()
		{
			this->InitializeCOM();

			HRESULT hresult = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), &this->pEnumerator);
			if (FAILED(hresult))
			{
				HEPH_RAISE_EXCEPTION(this, ExternalException(HEPH_FUNC, "An error occurred while initializing the audio device enumerator.", "WASAPI", WinAudioBase::GetComErrorMessage(hresult)));
				HEPHAUDIO_LOG("Device enumeration failed, terminating the device thread...", HEPH_CL_ERROR);
				return;
			}

			NativeAudio::CheckAudioDevices();

			while (!this->disposing) // if device enumeration fails prevent from destroying the pEnumerator
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			this->pEnumerator = nullptr;
			CoUninitialize();
		}
		void WinAudio::RenderData(AudioDevice* device, AudioFormatInfo format)
		{
			this->InitializeCOM();

			ComPtr<IAudioClient> pAudioClient = nullptr;
			ComPtr<IAudioRenderClient> pRenderClient = nullptr;
			ComPtr<IMMDevice> pDevice = nullptr;
			LPWSTR deviceId = nullptr;
			WAVEFORMATEXTENSIBLE* closestFormat = nullptr;
			WAVEFORMATEXTENSIBLE wfx = WinAudioBase::AFI2WFX(format);
			HANDLE hEvent = nullptr;
			UINT32 padding, nFramesAvailable, bufferSize;
			EncodedAudioBuffer mixedBuffer;
			void* renderBuffer = nullptr;
			HRESULT hres;

			if (device == nullptr || device->type != AudioDeviceType::Render || device->isDefault)
			{
				WINAUDIO_RENDER_THREAD_EXCPT(this->pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice), HEPH_FUNC, "An error occurred while getting the render device.");
			}
			else
			{
				WINAUDIO_RENDER_THREAD_EXCPT(this->pEnumerator->GetDevice(StringHelpers::StrToWide(device->id).c_str(), &pDevice), HEPH_FUNC, "An error occurred while getting the render device.");
			}

			WINAUDIO_RENDER_THREAD_EXCPT(pDevice->GetId(&deviceId), HEPH_FUNC, "An error occurred while getting the render device.");
			if (deviceId != nullptr)
			{
				this->renderDeviceId = StringHelpers::WideToStr(deviceId);
				CoTaskMemFree(deviceId);
				deviceId = nullptr;
			}
			WINAUDIO_RENDER_THREAD_EXCPT(pDevice->Activate(__uuidof(IAudioClient), this->params.renderClsCtx, nullptr, (void**)pAudioClient.GetAddressOf()), HEPH_FUNC, "An error occurred while activating the render device.");

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->IsFormatSupported(this->params.renderShareMode, (const WAVEFORMATEX*)&wfx, (WAVEFORMATEX**)&closestFormat), HEPH_FUNC, "An error occurred while checking if the given format is supported.");
			if (closestFormat != nullptr)
			{
				format = WinAudioBase::WFX2AFI(*closestFormat);
				wfx = *closestFormat;
				CoTaskMemFree(closestFormat);
				closestFormat = nullptr;
			}
			this->renderFormat = format;

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Initialize(this->params.renderShareMode,
				this->params.renderStreamFlags | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, // always use event callback
				WINAUDIO_MS_TO_REF_TIME(this->params.renderBufferDuration_ms),
				WINAUDIO_MS_TO_REF_TIME(this->params.renderPeriodicity_ms),
				(const WAVEFORMATEX*)&wfx,
				nullptr), HEPH_FUNC, "An error occurred while initializing the audio client.");

			WINAUDIO_RENDER_THREAD_EXCPT(pDevice->Activate(__uuidof(IAudioSessionManager), this->params.renderClsCtx, nullptr, (void**)this->pRenderSessionManager.GetAddressOf()), HEPH_FUNC, "An error occurred while activating the session manager.");
			pDevice = nullptr;

			WINAUDIO_RENDER_THREAD_EXCPT(this->pRenderSessionManager->GetAudioSessionControl(nullptr, 0, this->pRenderSessionControl.GetAddressOf()), HEPH_FUNC, "An error occurred while getting the session controls.");

			hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
			if (hEvent == nullptr)
			{
				WINAUDIO_RENDER_THREAD_EXCPT(E_FAIL, HEPH_FUNC, "An error occurred while setting the render event handle.");
			}
			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->SetEventHandle(hEvent), HEPH_FUNC, "An error occurred while setting the render event handle.");

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetBufferSize(&bufferSize), HEPH_FUNC, "An error occurred while rendering the samples.");

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)pRenderClient.GetAddressOf()), HEPH_FUNC, "An error occurred while rendering the samples.");
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(bufferSize, (BYTE**)&renderBuffer), HEPH_FUNC, "An error occurred while rendering the samples.");
			memset(renderBuffer, 0, (size_t)bufferSize * this->renderFormat.FrameSize());
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(bufferSize, 0), HEPH_FUNC, "An error occurred while rendering the samples.");

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Start(), HEPH_FUNC, "An error occurred while rendering the samples.");

			this->isRenderInitialized = true;

			while (!this->disposing && this->isRenderInitialized)
			{
				if (WaitForSingleObject(hEvent, 1000) != WAIT_OBJECT_0)
				{
					WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Stop(), HEPH_FUNC, "An error occurred while rendering the samples.");
					WINAUDIO_RENDER_THREAD_EXCPT(E_FAIL, HEPH_FUNC, "Render time-out.");
				}

				if (this->params.renderShareMode & AUDCLNT_SHAREMODE_EXCLUSIVE)
				{
					nFramesAvailable = bufferSize;
				}
				else
				{
					WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetCurrentPadding(&padding), HEPH_FUNC, "An error occurred while rendering the samples.");
					nFramesAvailable = bufferSize - padding;
				}

				if (nFramesAvailable > 0)
				{
					mixedBuffer = this->Mix(nFramesAvailable);

					WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(nFramesAvailable, (BYTE**)&renderBuffer), HEPH_FUNC, "An error occurred while rendering the samples.");
					(void)memcpy(renderBuffer, mixedBuffer.begin(), (size_t)nFramesAvailable * this->renderFormat.FrameSize());
					WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(nFramesAvailable, 0), HEPH_FUNC, "An error occurred while rendering the samples.");
				}
			}

			// wait for last samples to render
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Stop(), HEPH_FUNC, "An error occurred while rendering the samples.");

		RENDER_EXIT:
			pDevice = nullptr;
			this->pRenderSessionControl = nullptr;
			this->pRenderSessionManager = nullptr;
			if (hEvent != nullptr)
			{
				const BOOL eventResult = CloseHandle(hEvent);
				if (eventResult == FALSE)
				{
					HEPH_RAISE_EXCEPTION(this, Exception(HEPH_FUNC, "An error occurred while closing the render event handle."));
				}
				hEvent = nullptr;
			}
			pRenderClient = nullptr;
			pAudioClient = nullptr;
			CoUninitialize();
		}
		void WinAudio::CaptureData(AudioDevice* device, AudioFormatInfo format)
		{
			this->InitializeCOM();

			ComPtr<IAudioClient> pAudioClient;
			ComPtr<IAudioCaptureClient> pCaptureClient = nullptr;
			ComPtr<IMMDevice> pDevice = nullptr;
			LPWSTR deviceId = nullptr;
			WAVEFORMATEXTENSIBLE* closestFormat = nullptr;
			WAVEFORMATEXTENSIBLE wfx = WinAudioBase::AFI2WFX(format);
			UINT32 bufferSize, nFramesAvailable, packetLength;
			BYTE* captureBuffer = nullptr;
			DWORD flags = 0;
			size_t halfActualBufferDuration_ms;
			HRESULT hres;

			if (device == nullptr || device->type != AudioDeviceType::Capture || device->isDefault)
			{
				WINAUDIO_CAPTURE_THREAD_EXCPT(this->pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice), HEPH_FUNC, "An error occurred while getting the device.");
			}
			else
			{
				WINAUDIO_CAPTURE_THREAD_EXCPT(this->pEnumerator->GetDevice(StringHelpers::StrToWide(device->id).c_str(), &pDevice), HEPH_FUNC, "An error occurred while getting the device.");
			}

			WINAUDIO_CAPTURE_THREAD_EXCPT(pDevice->GetId(&deviceId), HEPH_FUNC, "An error occurred while getting the device.");
			if (deviceId != nullptr)
			{
				this->captureDeviceId = StringHelpers::WideToStr(deviceId);
				CoTaskMemFree(deviceId);
				deviceId = nullptr;
			}
			WINAUDIO_CAPTURE_THREAD_EXCPT(pDevice->Activate(__uuidof(IAudioClient), this->params.captureClsCtx, nullptr, (void**)pAudioClient.GetAddressOf()), HEPH_FUNC, "An error occurred while activating the device.");
			pDevice = nullptr;

			WINAUDIO_CAPTURE_THREAD_EXCPT(pAudioClient->IsFormatSupported(this->params.captureShareMode, (const WAVEFORMATEX*)&wfx, (WAVEFORMATEX**)&closestFormat), HEPH_FUNC, "An error occurred while checking if the given format is supported.");
			if (closestFormat != nullptr)
			{
				format = WinAudioBase::WFX2AFI(*closestFormat);
				wfx = *closestFormat;
				CoTaskMemFree(closestFormat);
				closestFormat = nullptr;
			}
			this->captureFormat = format;

			WINAUDIO_CAPTURE_THREAD_EXCPT(pAudioClient->Initialize(this->params.captureShareMode,
				this->params.captureStreamFlags & ~AUDCLNT_STREAMFLAGS_EVENTCALLBACK, // never use event callback
				WINAUDIO_MS_TO_REF_TIME(this->params.captureBufferDuration_ms),
				WINAUDIO_MS_TO_REF_TIME(this->params.capturePeriodicity_ms),
				(const WAVEFORMATEX*)&wfx,
				nullptr), HEPH_FUNC, "An error occurred while initializing the audio client.");

			WINAUDIO_CAPTURE_THREAD_EXCPT(pAudioClient->GetBufferSize(&bufferSize), HEPH_FUNC, "An error occurred while capturing the samples.");
			halfActualBufferDuration_ms = 500.0 * bufferSize / this->captureFormat.sampleRate;

			WINAUDIO_CAPTURE_THREAD_EXCPT(pAudioClient->GetService(__uuidof(IAudioCaptureClient), &pCaptureClient), HEPH_FUNC, "An error occurred while capturing the samples.");
			WINAUDIO_CAPTURE_THREAD_EXCPT(pAudioClient->Start(), HEPH_FUNC, "An error occurred while capturing the samples.");

			this->isCaptureInitialized = true;

			while (!this->disposing && this->isCaptureInitialized)
			{
				if (!this->isCapturePaused && this->OnCapture)
				{
					WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetNextPacketSize(&packetLength), HEPH_FUNC, "An error occurred while capturing the samples.");
					while (packetLength != 0)
					{
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetBuffer(&captureBuffer, &nFramesAvailable, &flags, nullptr, nullptr), HEPH_FUNC, "An error occurred while capturing the samples.");

						if (nFramesAvailable > 0)
						{
							EncodedAudioBuffer encodedBuffer((const uint8_t*)captureBuffer, nFramesAvailable * this->captureFormat.FrameSize(), this->captureFormat);
							AudioBuffer buffer = this->pAudioDecoder->Decode(encodedBuffer);

							AudioCaptureEventArgs captureEventArgs(this, buffer);
							this->OnCapture(&captureEventArgs, nullptr);
						}

						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->ReleaseBuffer(nFramesAvailable), HEPH_FUNC, "An error occurred while capturing the samples.");
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetNextPacketSize(&packetLength), HEPH_FUNC, "An error occurred while capturing the samples.");
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(halfActualBufferDuration_ms));
			}

			WINAUDIO_CAPTURE_THREAD_EXCPT(pAudioClient->Stop(), HEPH_FUNC, "An error occurred while capturing the samples.");

		CAPTURE_EXIT:
			pDevice = nullptr;
			pCaptureClient = nullptr;
			pAudioClient = nullptr;
			CoUninitialize();
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
				return AudioDeviceType::Null;
			}
		}
	}
}
#endif