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
#pragma region Audio Session Events
		WinAudio::AudioSessionEvents::AudioSessionEvents()
		{
			_cRef = 1;
			parent = nullptr;
			type = AudioDeviceType::Null;
		}
		ULONG STDMETHODCALLTYPE WinAudio::AudioSessionEvents::AddRef()
		{
			return InterlockedIncrement(&_cRef);
		}
		ULONG STDMETHODCALLTYPE WinAudio::AudioSessionEvents::Release()
		{
			ULONG ulRef = InterlockedDecrement(&_cRef);
			if (0 == ulRef)
			{
				delete this;
			}
			return ulRef;
		}
		long STDMETHODCALLTYPE WinAudio::AudioSessionEvents::QueryInterface(REFIID  riid, VOID** ppvInterface)
		{
			if (IID_IUnknown == riid)
			{
				AddRef();
				*ppvInterface = (IUnknown*)this;
			}
			else if (__uuidof(IAudioSessionEvents) == riid)
			{
				AddRef();
				*ppvInterface = (IAudioSessionEvents*)this;
			}
			else
			{
				*ppvInterface = NULL;
				return E_NOINTERFACE;
			}
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioSessionEvents::OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext)
		{
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioSessionEvents::OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext)
		{
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioSessionEvents::OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext)
		{
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioSessionEvents::OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext)
		{
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioSessionEvents::OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext)
		{
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioSessionEvents::OnStateChanged(AudioSessionState NewState)
		{
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioSessionEvents::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
		{
			return S_OK;
		}
#pragma endregion
#pragma region Audio Device Events
		WinAudio::AudioDeviceEvents::AudioDeviceEvents()
		{
			_cRef = 1;
			parent = nullptr;
		}
		ULONG STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::AddRef()
		{
			return InterlockedIncrement(&_cRef);
		}
		ULONG STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::Release()
		{
			ULONG ulRef = InterlockedDecrement(&_cRef);
			if (0 == ulRef)
			{
				delete this;
			}
			return ulRef;
		}
		long STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::QueryInterface(REFIID  riid, VOID** ppvInterface)
		{
			if (IID_IUnknown == riid)
			{
				AddRef();
				*ppvInterface = (IUnknown*)this;
			}
			else if (__uuidof(IAudioSessionEvents) == riid)
			{
				AddRef();
				*ppvInterface = (IAudioSessionEvents*)this;
			}
			else
			{
				*ppvInterface = NULL;
				return E_NOINTERFACE;
			}
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId)
		{
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::OnDeviceAdded(LPCWSTR pwstrDeviceId)
		{
			if (parent->OnAudioDeviceAdded)
			{
				AudioDevice device = parent->GetAudioDeviceById(pwstrDeviceId);
				AudioDeviceEventArgs deviceEventArgs = AudioDeviceEventArgs(parent, device);
				parent->OnAudioDeviceAdded(&deviceEventArgs, nullptr);
			}
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::OnDeviceRemoved(LPCWSTR pwstrDeviceId)
		{
			if (parent->OnAudioDeviceRemoved)
			{
				AudioDevice device = parent->GetAudioDeviceById(pwstrDeviceId);
				AudioDeviceEventArgs deviceEventArgs = AudioDeviceEventArgs(parent, device);
				parent->OnAudioDeviceRemoved(&deviceEventArgs, nullptr);
			}
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState)
		{
			switch (dwNewState)
			{
			case DEVICE_STATE_ACTIVE:
				OnDeviceAdded(pwstrDeviceId);
				break;
			case DEVICE_STATE_DISABLED:
			case DEVICE_STATE_UNPLUGGED:
			case DEVICE_STATE_NOTPRESENT:
				OnDeviceRemoved(pwstrDeviceId);
				break;
			}
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
		{
			return S_OK;
		}
#pragma endregion
		WinAudio::WinAudio() : NativeAudio()
		{
			if (!IsWindowsVistaOrGreater())
			{
				throw AudioException(E_NOINTERFACE, "WinAudio", "OS version must be at least Windows Vista.");
			}

			CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			HRESULT hres;
			WINAUDIO_EXCPT(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &pEnumerator), this, "WinAudio", "An error occurred whilst initializing the audio device enumerator.");

			deviceEvents.parent = this;
			WINAUDIO_EXCPT(pEnumerator->RegisterEndpointNotificationCallback(&deviceEvents), this, "WinAudio", "An error occurred whilst initializing the audio device enumerator.");

			hEvent = nullptr;
			pAudioClient = nullptr;
			pSessionManager = nullptr;
			pSessionControl = nullptr;
			sessionEvents.parent = this;
			sessionEvents.type = AudioDeviceType::Render;
		}
		WinAudio::~WinAudio()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG_LINE("Destructing WinAudio...", ConsoleLogger::info);

			disposing = true;

			JoinRenderThread();
			JoinCaptureThread();
			JoinQueueThreads();

			if (pSessionControl != nullptr)
			{
				pSessionControl->UnregisterAudioSessionNotification(&sessionEvents);
			}

			if (pEnumerator != nullptr)
			{
				pEnumerator->UnregisterEndpointNotificationCallback(&deviceEvents);
			}

			pEnumerator = nullptr;
			pSessionControl = nullptr;
			pSessionManager = nullptr;
			pAudioClient = nullptr;

			CoUninitialize();

			HEPHAUDIO_LOG_LINE("WinAudio destructed in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
		}
		void WinAudio::SetMasterVolume(HEPHAUDIO_DOUBLE volume)
		{
			if (pSessionManager != nullptr)
			{
				if (disposing) { return; }
				if (volume > 1.0) { volume = 1.0; }
				if (volume < 0.0) { volume = 0.0; }

				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				HRESULT hres;
				WINAUDIO_EXCPT(pSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), this, "WinAudio::SetMasterVolume", "An error occurred whilst setting the master volume.");

				WINAUDIO_EXCPT(pVolume->SetMasterVolume(volume, nullptr), this, "WinAudio::SetMasterVolume", "An error occurred whilst setting the master volume.");
			}
		}
		HEPHAUDIO_DOUBLE WinAudio::GetMasterVolume() const
		{
			if (disposing) { return -1.0; }

			float volume = -1.0f;

			if (pSessionManager != nullptr)
			{
				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				HRESULT hres;
				WINAUDIO_EXCPT(pSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), this, "WinAudio::GetMasterVolume", "An error occurred whilst getting the master volume.");

				WINAUDIO_EXCPT(pVolume->GetMasterVolume(&volume), this, "WinAudio::GetMasterVolume", "An error occurred whilst getting the master volume.");
			}

			return volume;
		}
		void WinAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG_LINE(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), ConsoleLogger::info);

			if (disposing) { return; }

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
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, nullptr, (void**)pAudioClient.GetAddressOf()), this, "WinAudio::InitializeRender", "An error occurred whilst activating the render device.");

			WAVEFORMATEX wrf = format;
			WINAUDIO_EXCPT(pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wrf, &closestFormat), this, "WinAudio::InitializeRender", "An error occurred whilst checking if the given format is supported.");
			if (closestFormat != nullptr) // The given format is not supported, initialize audio client using the closest format.
			{
				format = (*closestFormat);
				format.formatTag = 1;
				format.headerSize = 0;
				wrf = format;
				CoTaskMemFree(closestFormat);
			}
			renderFormat = format;

			WINAUDIO_EXCPT(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, &wrf, nullptr), this, "WinAudio::InitializeRender", "An error occurred whilst initializing the audio client.");
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, (void**)pSessionManager.GetAddressOf()), this, "WinAudio::InitializeRender", "An error occurred whilst activating the session manager.");

			WINAUDIO_EXCPT(pSessionManager->GetAudioSessionControl(nullptr, 0, pSessionControl.GetAddressOf()), this, "WinAudio::InitializeRender", "An error occurred whilst getting the session controls.");
			WINAUDIO_EXCPT(pSessionControl->RegisterAudioSessionNotification(&sessionEvents), this, "WinAudio::InitializeRender", "An error occurred whilst registering to session notifications.");

			hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			WINAUDIO_EXCPT(pAudioClient->SetEventHandle(hEvent), this, "WinAudio::InitializeRender", "An error occurred whilst setting the audio client event handle.");

			isRenderInitialized = true;
			renderThread = std::thread(&WinAudio::RenderData, this);

			HEPHAUDIO_LOG_LINE("Render initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
		}
		void WinAudio::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false;
				renderDeviceId = L"";
				JoinRenderThread();
				HRESULT hres;
				WINAUDIO_EXCPT(pSessionControl->UnregisterAudioSessionNotification(&sessionEvents), this, "WinAudio::StopRendering", "An error occurred whilst stopping render.");
				pSessionControl == nullptr;
				pSessionManager == nullptr;
				pAudioClient == nullptr;
				HEPHAUDIO_LOG_LINE("Stopped rendering.", ConsoleLogger::info);
			}
		}
		void WinAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG_LINE(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), ConsoleLogger::info);

			if (disposing) { return; }
			StopCapturing();

			ComPtr<IAudioClient3> pCaptureAudioClient = nullptr;
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
			if (closestFormat != nullptr) // The given format is not supported, initialize audio client using the closest format.
			{
				format = (*closestFormat);
				format.formatTag = 1;
				format.headerSize = 0;
				wcf = format;
				CoTaskMemFree(closestFormat);
			}
			captureFormat = format;

			WINAUDIO_EXCPT(pCaptureAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, &wcf, nullptr), this, "WinAudio::InitializeCapture", "An error occurred whilst initializing the audio client.");
			ComPtr<IAudioSessionManager2> pSessionManager = nullptr;
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, (void**)pSessionManager.GetAddressOf()), this, "WinAudio::InitializeCapture", "An error occurred whilst activating the session manager.");

			isCaptureInitialized = true;
			captureThread = std::thread(&WinAudio::CaptureData, this, pCaptureAudioClient, pSessionManager);

			HEPHAUDIO_LOG_LINE("Capture initialized in " + StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(StopWatch::milli), 4) + " ms.", ConsoleLogger::info);
		}
		void WinAudio::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false; // Set this to false to stop capturing.
				captureDeviceId = L"";
				JoinCaptureThread();
				HEPHAUDIO_LOG_LINE("Stopped capturing.", ConsoleLogger::info);
			}
		}
		void WinAudio::SetDisplayName(StringBuffer displayName)
		{
			if (!disposing && pSessionControl != nullptr)
			{
				this->displayName = displayName;
				HRESULT hres;
				WINAUDIO_EXCPT(pSessionControl->SetDisplayName(displayName.fwc_str(), nullptr), this, "WinAudio::SetDisplayName", "An error occurred whilst setting the display name.");
			}
		}
		void WinAudio::SetIconPath(StringBuffer iconPath)
		{
			if (pSessionControl != nullptr)
			{
				this->iconPath = iconPath;
				HRESULT hres;
				WINAUDIO_EXCPT(pSessionControl->SetIconPath(iconPath.fwc_str(), nullptr), this, "WinAudio::SetIconPath", "An error occurred whilst setting the icon path.");
			}
		}
		AudioDevice WinAudio::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			if (disposing) { return AudioDevice(); }

			HRESULT hres;

			if (deviceType == AudioDeviceType::All || deviceType == AudioDeviceType::Null)
			{
				WINAUDIO_EXCPT(E_INVALIDARG, this, "WinAudio::GetDefaultAudioDevice", "DeviceType must be either Render or Capture.");
			}

			const EDataFlow dataFlow = DeviceTypeToDataFlow(deviceType);
			AudioDevice defaultDevice;
			ComPtr<IMMDevice> pDefaultDevice = nullptr;
			ComPtr<IPropertyStore> pPropertyStore = nullptr;

			WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(dataFlow, eConsole, &pDefaultDevice), this, "WinAudio::GetDefaultAudioDevice", "An error occurred whilst getting the default device.");
			LPWSTR defaultDeviceId;
			WINAUDIO_EXCPT(pDefaultDevice->GetId(&defaultDeviceId), this, "WinAudio::GetDefaultAudioDevice", "An error occurred whilst getting default device id.");

			// Open the property store of the device to read device info.
			WINAUDIO_EXCPT(pDefaultDevice->OpenPropertyStore(STGM_READ, &pPropertyStore), this, "WinAudio::GetDefaultAudioDevice", "An error occurred whilst reading the default devices properties.");
			PROPVARIANT variant;
			PropVariantInit(&variant);

			// Read device name.
			WINAUDIO_EXCPT(pPropertyStore->GetValue(PKEY_Device_FriendlyName, &variant), this, "WinAudio::GetDefaultAudioDevice", "An error occurred whilst reading the default devices properties.");
			if (variant.vt != VT_EMPTY)
			{
				defaultDevice.name = variant.pwszVal;
				PropVariantClear(&variant);

			}
			// Read device description.
			WINAUDIO_EXCPT(pPropertyStore->GetValue(PKEY_Device_DeviceDesc, &variant), this, "WinAudio::GetDefaultAudioDevice", "An error occurred whilst reading the default devices properties.");
			if (variant.vt != VT_EMPTY)
			{
				defaultDevice.description = variant.pwszVal;
				PropVariantClear(&variant);
			}

			defaultDevice.id = defaultDeviceId;
			defaultDevice.type = deviceType;
			defaultDevice.isDefault = true;

			if (defaultDeviceId != nullptr)
			{
				CoTaskMemFree(defaultDeviceId);
			}

			return defaultDevice;
		}
		std::vector<AudioDevice> WinAudio::GetAudioDevices(AudioDeviceType deviceType) const
		{
			if (disposing) { return { }; }

			HRESULT hres;

			if (deviceType == AudioDeviceType::Null)
			{
				WINAUDIO_EXCPT(E_INVALIDARG, this, "WinAudio::GetAudioDevices", "DeviceType must not be Null.");
			}

			const EDataFlow dataFlow = DeviceTypeToDataFlow(deviceType);
			std::vector<AudioDevice> devices;
			ComPtr<IMMDeviceCollection> pCollection = nullptr;
			ComPtr<IMMDevice> pDefaultRender = nullptr;
			ComPtr<IMMDevice> pDefaultCapture = nullptr;
			LPWSTR defaultRenderId = nullptr;
			LPWSTR defaultCaptureId = nullptr;

			WINAUDIO_EXCPT(pEnumerator->EnumAudioEndpoints(dataFlow, DEVICE_STATE_ACTIVE, &pCollection), this, "WinAudio::GetAudioDevices", "An error occurred whilst enumerating the devices.");

			if ((deviceType & AudioDeviceType::Render) == AudioDeviceType::Render) // Get default render devices id, we will use it later to identify the default render device.
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultRender), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default render device id.");
				WINAUDIO_EXCPT(pDefaultRender->GetId(&defaultRenderId), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default render device id.");
			}
			StringBuffer defaultRenderIdStr = defaultRenderId != nullptr ? defaultRenderId : L"";

			if ((deviceType & AudioDeviceType::Capture) == AudioDeviceType::Capture) // Get default capture devices id, we will use it later to identify the default capture device.
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultCapture), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default capture device id.");
				WINAUDIO_EXCPT(pDefaultCapture->GetId(&defaultCaptureId), this, "WinAudio::GetAudioDevices", "An error occurred whilst getting the default capture device id.");
			}
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

				// Read device description.
				WINAUDIO_EXCPT(pPropertyStore->GetValue(PKEY_Device_DeviceDesc, &variant), this, "WinAudio::GetAudioDevices", "An error occurred whilst reading the devices properties.");
				if (variant.vt != VT_EMPTY)
				{
					device.description = variant.pwszVal;
					PropVariantClear(&variant);
				}

				device.id = deviceId;
				device.type = DataFlowToDeviceType(dataFlow);
				device.isDefault = (device.type == AudioDeviceType::Render && device.id == defaultRenderIdStr) || (device.type == AudioDeviceType::Capture && device.id == defaultCaptureIdStr);
				devices.push_back(device);

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

			return devices;
		}
		UINT32 WinAudio::GetRenderBufferSize() const
		{
			if (disposing) { return 0; }
			UINT32 bufferSize;
			HRESULT hres;
			WINAUDIO_EXCPT(pAudioClient->GetBufferSize(&bufferSize), this, "WinAudio", "An error occurred whilst getting the render buffer size.");
			return bufferSize;
		}
		void WinAudio::RenderData()
		{
			ComPtr<IAudioRenderClient> pRenderClient = nullptr;
			AudioBuffer dataBuffer;
			UINT32 padding, nFramesAvailable;
			BYTE* renderBuffer;
			UINT32 bufferSize;
			HRESULT hres;

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetBufferSize(&bufferSize), this, "WinAudio", "An error occurred whilst rendering the samples.");
			dataBuffer = AudioBuffer(bufferSize, renderFormat);

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetService(__uuidof(IAudioRenderClient), &pRenderClient), this, "WinAudio", "An error occurred whilst rendering the samples.");
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(bufferSize, &renderBuffer), this, "WinAudio", "An error occurred whilst rendering the samples.");
			memset(renderBuffer, 0, bufferSize * renderFormat.FrameSize());
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(bufferSize, 0), this, "WinAudio", "An error occurred whilst rendering the samples.");

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Start(), this, "WinAudio", "An error occurred whilst rendering the samples.");

			while (!disposing && isRenderInitialized)
			{
				uint32_t retval = WaitForSingleObject(hEvent, 2000);
				if (retval != WAIT_OBJECT_0)
				{
					WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Stop(), this, "WinAudio", "An error occurred whilst rendering the samples.");
					WINAUDIO_RENDER_THREAD_EXCPT(E_FAIL, this, "WinAudio", "An error occurred whilst rendering the samples.");
				}

				WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetCurrentPadding(&padding), this, "WinAudio", "An error occurred whilst rendering the samples.");
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

			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Stop(), this, "WinAudio", "An error occurred whilst rendering the samples.");

		RENDER_EXIT:
			if (hEvent != nullptr)
			{
				CloseHandle(hEvent);
				hEvent = nullptr;
			}
		}
		void WinAudio::CaptureData(ComPtr<IAudioClient3> pCaptureAudioClient, ComPtr<IAudioSessionManager2> pSessionManager)
		{
			constexpr REFERENCE_TIME reftimesPerSec = 1e6;
			constexpr REFERENCE_TIME reftimesPerMilliSec = 2e3;

			ComPtr<IAudioCaptureClient> pCaptureClient = nullptr;
			ComPtr<IAudioSessionControl> pSessionControl = nullptr;
			UINT32 bufferSize, nFramesAvailable, packetLength;
			BYTE* captureBuffer = nullptr;
			DWORD flags = 0;
			REFERENCE_TIME hnsActualDuration = 0;
			AudioSessionEvents captureSessionEvents;
			captureSessionEvents.parent = this;
			captureSessionEvents.type = AudioDeviceType::Capture;
			HRESULT hres;

			WINAUDIO_CAPTURE_THREAD_EXCPT(pSessionManager->GetAudioSessionControl(nullptr, 0, pSessionControl.GetAddressOf()), this, "WinAudio", "An error occurred whilst getting the session controls.");
			WINAUDIO_CAPTURE_THREAD_EXCPT(pSessionControl->RegisterAudioSessionNotification(&captureSessionEvents), this, "WinAudio", "An error occurred whilst registering to session notifications.");

			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->GetBufferSize(&bufferSize), this, "WinAudio", "An error occurred whilst capturing the samples.");
			hnsActualDuration = (HEPHAUDIO_DOUBLE)reftimesPerSec * bufferSize / captureFormat.sampleRate;

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
		CAPTURE_EXIT:
			WINAUDIO_CAPTURE_THREAD_EXCPT(pSessionControl->UnregisterAudioSessionNotification(&captureSessionEvents), this, "WinAudio", "An error occurred whilst capturing the samples.");
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