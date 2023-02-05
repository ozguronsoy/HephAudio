#ifdef _WIN32
#include "WinAudio.h"
#include <AudioProcessor.h>
#include <VersionHelpers.h>

using namespace Microsoft::WRL;

#define WINAUDIO_EXCPT(hr, winAudio, method, message) if(FAILED(hr)) { RAISE_AUDIO_EXCPT(winAudio, AudioException(hr, method, message)); throw AudioException(hr, method, message); }
#define WINAUDIO_RENDER_THREAD_EXCPT(hr, winAudio, method, message) if(FAILED(hr)) { RAISE_AUDIO_EXCPT(winAudio, AudioException(hr, method, message)); goto RENDER_EXIT; }
#define WINAUDIO_CAPTURE_THREAD_EXCPT(hr, winAudio, method, message) if(FAILED(hr)) { RAISE_AUDIO_EXCPT(winAudio, AudioException(hr, method, message)); goto CAPTURE_EXIT; }

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
			if (DisconnectReason == AudioSessionDisconnectReason::DisconnectReasonDeviceRemoval)
			{
				if (type == AudioDeviceType::Render)
				{
					parent->StopRendering();
					parent->InitializeRender(nullptr, parent->renderFormat);
					if (parent->displayName != L"")
					{
						parent->SetDisplayName(parent->displayName);
					}
					if (parent->iconPath != L"")
					{
						parent->SetIconPath(parent->iconPath);
					}
				}
				else if (type == AudioDeviceType::Capture)
				{
					parent->StopCapturing();
					parent->InitializeCapture(nullptr, parent->captureFormat);
				}
			}
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
			if (parent->OnDefaultAudioDeviceChange != nullptr)
			{
				parent->OnDefaultAudioDeviceChange(parent->GetAudioDeviceById(pwstrDeviceId));
			}
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::OnDeviceAdded(LPCWSTR pwstrDeviceId)
		{
			if (parent->OnAudioDeviceAdded != nullptr)
			{
				parent->OnAudioDeviceAdded(parent->GetAudioDeviceById(pwstrDeviceId));
			}
			return S_OK;
		}
		long STDMETHODCALLTYPE WinAudio::AudioDeviceEvents::OnDeviceRemoved(LPCWSTR pwstrDeviceId)
		{
			if (parent->OnAudioDeviceRemoved != nullptr)
			{
				parent->OnAudioDeviceRemoved(parent->GetAudioDeviceById(pwstrDeviceId));
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
				throw AudioException(E_NOINTERFACE, L"WinAudio", L"OS version must be at least Windows Vista.");
			}
			CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			WINAUDIO_EXCPT(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &pEnumerator), this, L"WinAudio", L"An error occurred whilst initializing the audio device enumerator.");
			deviceEvents.parent = this;
			WINAUDIO_EXCPT(pEnumerator->RegisterEndpointNotificationCallback(&deviceEvents), this, L"WinAudio", L"An error occurred whilst initializing the audio device enumerator.");
			hEvent = nullptr;
			pAudioClient = nullptr;
			pSessionManager = nullptr;
			pSessionControl = nullptr;
			sessionEvents.parent = this;
			sessionEvents.type = AudioDeviceType::Render;
		}
		WinAudio::~WinAudio()
		{
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
			// Release before uninitializing com.
			pEnumerator = nullptr;
			pSessionControl = nullptr;
			pSessionManager = nullptr;
			pAudioClient = nullptr;
			CoUninitialize();
		}
		void WinAudio::SetMasterVolume(double volume)
		{
			if (pSessionManager != nullptr)
			{
				if (disposing) { return; }
				if (volume > 1.0) { volume = 1.0; }
				if (volume < 0.0) { volume = 0.0; }
				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				WINAUDIO_EXCPT(pSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), this, L"WinAudio::SetMasterVolume", L"An error occurred whilst setting the master volume.");
				WINAUDIO_EXCPT(pVolume->SetMasterVolume(volume, nullptr), this, L"WinAudio::SetMasterVolume", L"An error occurred whilst setting the master volume.");
			}
		}
		double WinAudio::GetMasterVolume() const
		{
			if (disposing) { return -1.0; }
			float volume = -1.0f;
			if (pSessionManager != nullptr)
			{
				ComPtr<ISimpleAudioVolume> pVolume = nullptr;
				WINAUDIO_EXCPT(pSessionManager->GetSimpleAudioVolume(nullptr, 0, &pVolume), this, L"WinAudio::GetMasterVolume", L"An error occurred whilst getting the master volume.");
				WINAUDIO_EXCPT(pVolume->GetMasterVolume(&volume), this, L"WinAudio::GetMasterVolume", L"An error occurred whilst getting the master volume.");
			}
			return volume;
		}
		void WinAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			if (disposing) { return; }
			StopRendering();
			ComPtr<IMMDevice> pDevice = nullptr;
			WAVEFORMATEX* closestFormat = nullptr;
			if (device == nullptr || device->type != AudioDeviceType::Render || device->isDefault)
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice), this, L"WinAudio::InitializeRender", L"An error occurred whilst getting the render device.");
			}
			else
			{
				WINAUDIO_EXCPT(pEnumerator->GetDevice(device->id.c_str(), &pDevice), this, L"WinAudio::InitializeRender", L"An error occurred whilst getting the render device.");
			}
			LPWSTR deviceId = nullptr;
			WINAUDIO_EXCPT(pDevice->GetId(&deviceId), this, L"WinAudio::InitializeRender", L"An error occurred whilst getting the render device.");
			if (deviceId != nullptr)
			{
				renderDeviceId = std::wstring(deviceId);
				CoTaskMemFree(deviceId);
			}
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, nullptr, (void**)pAudioClient.GetAddressOf()), this, L"WinAudio::InitializeRender", L"An error occurred whilst activating the render device.");
			WAVEFORMATEX wrf = format;
			WINAUDIO_EXCPT(pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wrf, &closestFormat), this, L"WinAudio::InitializeRender", L"An error occurred whilst checking if the given format is supported.");
			if (closestFormat != nullptr) // The given format is not supported, initialize audio client using the closest format.
			{
				format = (*closestFormat);
				format.formatTag = 1;
				format.headerSize = 0;
				wrf = format;
				CoTaskMemFree(closestFormat);
			}
			renderFormat = format;
			WINAUDIO_EXCPT(pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, &wrf, nullptr), this, L"WinAudio::InitializeRender", L"An error occurred whilst initializing the audio client.");
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, (void**)pSessionManager.GetAddressOf()), this, L"WinAudio::InitializeRender", L"An error occurred whilst activating the session manager.");
			WINAUDIO_EXCPT(pSessionManager->GetAudioSessionControl(nullptr, 0, pSessionControl.GetAddressOf()), this, L"WinAudio::InitializeRender", L"An error occurred whilst getting the session controls.");
			WINAUDIO_EXCPT(pSessionControl->RegisterAudioSessionNotification(&sessionEvents), this, L"WinAudio::InitializeRender", L"An error occurred whilst registering to session notifications.");
			hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			WINAUDIO_EXCPT(pAudioClient->SetEventHandle(hEvent), this, L"WinAudio::InitializeRender", L"An error occurred whilst setting the audio client event handle.");
			isRenderInitialized = true;
			renderThread = std::thread(&WinAudio::RenderData, this);
		}
		void WinAudio::StopRendering()
		{
			if (isRenderInitialized)
			{
				isRenderInitialized = false; // Set this to false to stop rendering.
				renderDeviceId = L"";
				JoinRenderThread();
				WINAUDIO_EXCPT(pSessionControl->UnregisterAudioSessionNotification(&sessionEvents), this, L"WinAudio::StopRendering", L"An error occurred whilst stopping render.");
				pSessionControl == nullptr;
				pSessionManager == nullptr;
				pAudioClient == nullptr;
			}
		}
		void WinAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			if (disposing) { return; }
			StopCapturing();
			ComPtr<IAudioClient3> pCaptureAudioClient = nullptr;
			ComPtr<IMMDevice> pDevice = nullptr;
			WAVEFORMATEX* closestFormat = nullptr;
			if (device == nullptr || device->type != AudioDeviceType::Capture || device->isDefault)
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice), this, L"WinAudio::InitializeCapture", L"An error occurred whilst getting the device.");
			}
			else
			{
				WINAUDIO_EXCPT(pEnumerator->GetDevice(device->id.c_str(), &pDevice), this, L"WinAudio::InitializeCapture", L"An error occurred whilst getting the device.");
			}
			LPWSTR deviceId = nullptr;
			WINAUDIO_EXCPT(pDevice->GetId(&deviceId), this, L"WinAudio::InitializeCapture", L"An error occurred whilst getting the device.");
			if (deviceId != nullptr)
			{
				captureDeviceId = std::wstring(deviceId);
				CoTaskMemFree(deviceId);
			}
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioClient3), CLSCTX_ALL, nullptr, (void**)pCaptureAudioClient.GetAddressOf()), this, L"WinAudio::InitializeCapture", L"An error occurred whilst activating the device.");
			WAVEFORMATEX wcf = format;
			WINAUDIO_EXCPT(pCaptureAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wcf, &closestFormat), this, L"WinAudio::InitializeCapture", L"An error occurred whilst checking if the given format is supported.");
			if (closestFormat != nullptr) // The given format is not supported, initialize audio client using the closest format.
			{
				format = (*closestFormat);
				format.formatTag = 1;
				format.headerSize = 0;
				wcf = format;
				CoTaskMemFree(closestFormat);
			}
			captureFormat = format;
			WINAUDIO_EXCPT(pCaptureAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10000000, 0, &wcf, nullptr), this, L"WinAudio::InitializeCapture", L"An error occurred whilst initializing the audio client.");
			ComPtr<IAudioSessionManager2> pSessionManager = nullptr;
			WINAUDIO_EXCPT(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER, nullptr, (void**)pSessionManager.GetAddressOf()), this, L"WinAudio::InitializeCapture", L"An error occurred whilst activating the session manager.");
			isCaptureInitialized = true;
			captureThread = std::thread(&WinAudio::CaptureData, this, pCaptureAudioClient, pSessionManager);
		}
		void WinAudio::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				isCaptureInitialized = false; // Set this to false to stop capturing.
				captureDeviceId = L"";
				JoinCaptureThread();
			}
		}
		void WinAudio::SetDisplayName(std::wstring displayName)
		{
			if (!disposing && pSessionControl != nullptr)
			{
				this->displayName = displayName;
				WINAUDIO_EXCPT(pSessionControl->SetDisplayName(displayName.c_str(), nullptr), this, L"WinAudio::SetDisplayName", L"An error occurred whilst setting the display name.");
			}
		}
		void WinAudio::SetIconPath(std::wstring iconPath)
		{
			if (pSessionControl != nullptr)
			{
				this->iconPath = iconPath;
				WINAUDIO_EXCPT(pSessionControl->SetIconPath(iconPath.c_str(), nullptr), this, L"WinAudio::SetIconPath", L"An error occurred whilst setting the icon path.");
			}
		}
		AudioDevice WinAudio::GetDefaultAudioDevice(AudioDeviceType deviceType) const
		{
			if (disposing) { return AudioDevice(); }
			if (deviceType == AudioDeviceType::All || deviceType == AudioDeviceType::Null)
			{
				WINAUDIO_EXCPT(E_INVALIDARG, this, L"WinAudio::GetDefaultAudioDevice", L"DeviceType must be either Render or Capture.");
			}
			const EDataFlow dataFlow = DeviceTypeToDataFlow(deviceType);
			AudioDevice defaultDevice;
			ComPtr<IMMDevice> pDefaultDevice = nullptr;
			ComPtr<IPropertyStore> pPropertyStore = nullptr;
			WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(dataFlow, eConsole, &pDefaultDevice), this, L"WinAudio::GetDefaultAudioDevice", L"An error occurred whilst getting the default device.");
			LPWSTR defaultDeviceId;
			WINAUDIO_EXCPT(pDefaultDevice->GetId(&defaultDeviceId), this, L"WinAudio::GetDefaultAudioDevice", L"An error occurred whilst getting default device id.");
			// Open the property store of the device to read device info.
			WINAUDIO_EXCPT(pDefaultDevice->OpenPropertyStore(STGM_READ, &pPropertyStore), this, L"WinAudio::GetDefaultAudioDevice", L"An error occurred whilst reading the default devices properties.");
			PROPVARIANT variant;
			PropVariantInit(&variant);
			// Read device name.
			WINAUDIO_EXCPT(pPropertyStore->GetValue(PKEY_Device_FriendlyName, &variant), this, L"WinAudio::GetDefaultAudioDevice", L"An error occurred whilst reading the default devices properties.");
			if (variant.vt != VT_EMPTY) // if name found.
			{
				defaultDevice.name = variant.pwszVal;
				PropVariantClear(&variant); // Free memory.
			}
			// Read device description.
			WINAUDIO_EXCPT(pPropertyStore->GetValue(PKEY_Device_DeviceDesc, &variant), this, L"WinAudio::GetDefaultAudioDevice", L"An error occurred whilst reading the default devices properties.");
			if (variant.vt != VT_EMPTY) // if description found.
			{
				defaultDevice.description = variant.pwszVal;
				PropVariantClear(&variant); // Free memory.
			}
			defaultDevice.id = defaultDeviceId;
			defaultDevice.type = deviceType;
			defaultDevice.isDefault = true;
			if (defaultDeviceId != nullptr) // Free memory.
			{
				CoTaskMemFree(defaultDeviceId);
			}
			return defaultDevice;
		}
		std::vector<AudioDevice> WinAudio::GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const
		{
			if (disposing) { return { }; }
			if (deviceType == AudioDeviceType::Null)
			{
				WINAUDIO_EXCPT(E_INVALIDARG, this, L"WinAudio::GetAudioDevices", L"DeviceType must not be Null.");
			}
			const EDataFlow dataFlow = DeviceTypeToDataFlow(deviceType);
			std::vector<AudioDevice> devices;
			ComPtr<IMMDeviceCollection> pCollection = nullptr;
			ComPtr<IMMDevice> pDefaultRender = nullptr;
			ComPtr<IMMDevice> pDefaultCapture = nullptr;
			LPWSTR defaultRenderId = nullptr;
			LPWSTR defaultCaptureId = nullptr;
			uint32_t deviceState = DEVICE_STATE_ACTIVE;
			if (includeInactive)
			{
				deviceState = DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED | DEVICE_STATE_UNPLUGGED;
			}
			WINAUDIO_EXCPT(pEnumerator->EnumAudioEndpoints(dataFlow, deviceState, &pCollection), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst enumerating the devices.");
			if ((deviceType & AudioDeviceType::Render) == AudioDeviceType::Render) // Get default render devices id, we will use it later to identify the default render device.
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultRender), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst getting the default render device id.");
				WINAUDIO_EXCPT(pDefaultRender->GetId(&defaultRenderId), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst getting the default render device id.");
			}
			std::wstring defaultRenderIdStr = defaultRenderId != nullptr ? defaultRenderId : L"";
			if ((deviceType & AudioDeviceType::Capture) == AudioDeviceType::Capture) // Get default capture devices id, we will use it later to identify the default capture device.
			{
				WINAUDIO_EXCPT(pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultCapture), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst getting the default capture device id.");
				WINAUDIO_EXCPT(pDefaultCapture->GetId(&defaultCaptureId), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst getting the default capture device id.");
			}
			std::wstring defaultCaptureIdStr = defaultCaptureId != nullptr ? defaultCaptureId : L"";
			UINT deviceCount = 0;
			WINAUDIO_EXCPT(pCollection->GetCount(&deviceCount), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst getting the device count.");
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
				WINAUDIO_EXCPT(pCollection->Item(i, &pDevice), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst reading the devices properties.");
				WINAUDIO_EXCPT(pDevice->GetId(&deviceId), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst getting the device id.");
				// Get device data flow.
				WINAUDIO_EXCPT(pDevice->QueryInterface(__uuidof(IMMEndpoint), &pEndpoint), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst getting the device type.");
				WINAUDIO_EXCPT(pEndpoint->GetDataFlow(&dataFlow), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst getting the device type.");
				// Open the property store of the device to read device info.
				WINAUDIO_EXCPT(pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst reading the devices properties.");
				WINAUDIO_EXCPT(pPropertyStore->GetValue(PKEY_Device_FriendlyName, &variant), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst reading the devices properties.");
				// Read device name.
				if (variant.vt != VT_EMPTY) // if name found.
				{
					device.name = variant.pwszVal;
					PropVariantClear(&variant); // Free memory.
				}
				// Read device description.
				WINAUDIO_EXCPT(pPropertyStore->GetValue(PKEY_Device_DeviceDesc, &variant), this, L"WinAudio::GetAudioDevices", L"An error occurred whilst reading the devices properties.");
				if (variant.vt != VT_EMPTY) // if description found.
				{
					device.description = variant.pwszVal;
					PropVariantClear(&variant); // Free memory.
				}
				device.id = deviceId;
				device.type = DataFlowToDeviceType(dataFlow);
				device.isDefault = (device.type == AudioDeviceType::Render && device.id == defaultRenderIdStr) || (device.type == AudioDeviceType::Capture && device.id == defaultCaptureIdStr);
				devices.push_back(device);
				if (deviceId != nullptr) // Free memory.
				{
					CoTaskMemFree(deviceId);
				}
			}
			if (defaultRenderId != nullptr) // Free memory.
			{
				CoTaskMemFree(defaultRenderId);
			}
			if (defaultCaptureId != nullptr) // Free memory.
			{
				CoTaskMemFree(defaultCaptureId);
			}
			return devices;
		}
		UINT32 WinAudio::GetRenderBufferSize() const
		{
			if (disposing) { return 0; }
			UINT32 bufferSize;
			WINAUDIO_EXCPT(pAudioClient->GetBufferSize(&bufferSize), this, L"WinAudio", L"An error occurred whilst getting the render buffer size.");
			return bufferSize;
		}
		void WinAudio::RenderData()
		{
			ComPtr<IAudioRenderClient> pRenderClient = nullptr;
			AudioBuffer dataBuffer;
			UINT32 padding, nFramesAvailable;
			BYTE* renderBuffer;
			UINT32 bufferSize;
			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetBufferSize(&bufferSize), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
			dataBuffer = AudioBuffer(bufferSize, renderFormat);
			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetService(__uuidof(IAudioRenderClient), &pRenderClient), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(bufferSize, &renderBuffer), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
			memset(renderBuffer, 0, bufferSize * renderFormat.FrameSize());
			WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(bufferSize, 0), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Start(), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
			while (!disposing && isRenderInitialized) // Render data while not disposing and not reinitializing.
			{
				// Wait for next buffer event to be signaled.
				uint32_t retval = WaitForSingleObject(hEvent, 2000);
				if (retval != WAIT_OBJECT_0)
				{
					WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Stop(), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
					WINAUDIO_RENDER_THREAD_EXCPT(E_FAIL, this, L"WinAudio", L"An error occurred whilst rendering the samples.");
				}
				WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->GetCurrentPadding(&padding), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
				nFramesAvailable = bufferSize - padding;
				if (nFramesAvailable > 0)
				{
					Mix(dataBuffer, nFramesAvailable);
					WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->GetBuffer(nFramesAvailable, &renderBuffer), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
					memcpy(renderBuffer, dataBuffer.Begin(), nFramesAvailable * renderFormat.FrameSize());
					WINAUDIO_RENDER_THREAD_EXCPT(pRenderClient->ReleaseBuffer(nFramesAvailable, 0), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
					dataBuffer.Reset();
				}
			}
			WINAUDIO_RENDER_THREAD_EXCPT(pAudioClient->Stop(), this, L"WinAudio", L"An error occurred whilst rendering the samples.");
		RENDER_EXIT:
			if (hEvent != nullptr)
			{
				CloseHandle(hEvent);
				hEvent = nullptr;
			}
		}
		void WinAudio::CaptureData(ComPtr<IAudioClient3> pCaptureAudioClient, ComPtr<IAudioSessionManager2> pSessionManager)
		{
			ComPtr<IAudioCaptureClient> pCaptureClient = nullptr;
			ComPtr<IAudioSessionControl> pSessionControl = nullptr;
			UINT32 bufferSize, nFramesAvailable, nBytesAvailable, packetLength;
			BYTE* captureBuffer = nullptr;
			DWORD flags = 0;
			const REFERENCE_TIME reftimesPerSec = 10000000;
			const REFERENCE_TIME reftimesPerMilliSec = 10000;
			REFERENCE_TIME hnsActualDuration = 0;
			AudioSessionEvents captureSessionEvents;
			AudioBuffer capturedData(0, captureFormat);
			captureSessionEvents.parent = this;
			captureSessionEvents.type = AudioDeviceType::Capture;
			WINAUDIO_CAPTURE_THREAD_EXCPT(pSessionManager->GetAudioSessionControl(nullptr, 0, pSessionControl.GetAddressOf()), this, L"WinAudio::InitializeCapture", L"An error occurred whilst getting the session controls.");
			WINAUDIO_CAPTURE_THREAD_EXCPT(pSessionControl->RegisterAudioSessionNotification(&captureSessionEvents), this, L"WinAudio::InitializeCapture", L"An error occurred whilst registering to session notifications.");
			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->GetBufferSize(&bufferSize), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
			hnsActualDuration = (double)reftimesPerSec * bufferSize / captureFormat.sampleRate;
			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->GetService(__uuidof(IAudioCaptureClient), &pCaptureClient), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->Start(), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
			while (!disposing && isCaptureInitialized) // Capture data while not disposing and not reinitializing.
			{
				if (!isCapturePaused) // and not paused.
				{
					capturedData = AudioBuffer(0, AudioFormatInfo(37, captureFormat.channelCount, sizeof(double) * 8, captureFormat.sampleRate));
					Sleep(hnsActualDuration / reftimesPerMilliSec / 2);
					WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetNextPacketSize(&packetLength), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
					while (packetLength != 0)
					{
						if (isCapturePaused || disposing) { break; } // Stop capturing if paused or disposing.
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetBuffer(&captureBuffer, &nFramesAvailable, &flags, nullptr, nullptr), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
						nBytesAvailable = nFramesAvailable * captureFormat.FrameSize();
						AudioBuffer temp(nFramesAvailable, captureFormat);
						memcpy(temp.Begin(), captureBuffer, nBytesAvailable);
						AudioProcessor::ConvertPcmToInnerFormat(temp);
						capturedData += temp;
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->ReleaseBuffer(nFramesAvailable), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
						WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureClient->GetNextPacketSize(&packetLength), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
					}
					if (OnCapture != nullptr)
					{
						OnCapture(capturedData);
					}
				}
			}
			WINAUDIO_CAPTURE_THREAD_EXCPT(pCaptureAudioClient->Stop(), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
		CAPTURE_EXIT:
			WINAUDIO_CAPTURE_THREAD_EXCPT(pSessionControl->UnregisterAudioSessionNotification(&captureSessionEvents), this, L"WinAudio", L"An error occurred whilst capturing the samples.");
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