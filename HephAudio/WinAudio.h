#pragma once
#ifdef _WIN32
#include "framework.h"
#include "NativeAudio.h"
#include <wrl.h>
#include <Mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <mmeapi.h>

namespace HephAudio
{
	namespace Native
	{
		/// <summary>
		/// Uses WASAPI. Use WinAudioDS if you have Windows XP or lower.
		/// </summary>
		class WinAudio : public NativeAudio
		{
#pragma region Audio Session Events
		protected:
			/// <summary>
			/// Implementation of the IAudioSessionEvents interface.
			/// </summary>
			class AudioSessionEvents final : public IAudioSessionEvents
			{
			private:
				LONG _cRef;
			public:
				WinAudio* parent;
				AudioDeviceType type;
			public:
				AudioSessionEvents();
				ULONG STDMETHODCALLTYPE AddRef();
				ULONG STDMETHODCALLTYPE Release();
				long STDMETHODCALLTYPE QueryInterface(REFIID  riid, VOID** ppvInterface);
				long STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext);
				long STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext);
				long STDMETHODCALLTYPE OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext);
				long STDMETHODCALLTYPE OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext);
				long STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext);
				long STDMETHODCALLTYPE OnStateChanged(AudioSessionState NewState);
				long STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason);
			};
#pragma endregion
#pragma region Audio Device Events
		protected:
			/// <summary>
			/// Implementation of the IMMNotificationClient interface.
			/// </summary>
			class AudioDeviceEvents final : public IMMNotificationClient
			{
			private:
				LONG _cRef;
			public:
				WinAudio* parent;
			public:
				AudioDeviceEvents();
				ULONG STDMETHODCALLTYPE AddRef();
				ULONG STDMETHODCALLTYPE Release();
				long STDMETHODCALLTYPE QueryInterface(REFIID  riid, VOID** ppvInterface);
				long STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId);
				long STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
				long STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
				long STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
				long STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);
			};
#pragma endregion
		protected:
			/// <summary>
			/// The event handle for the render client interface.
			/// </summary>
			HANDLE hEvent;
			/// <summary>
			/// The WASAPI interface that handles the device enumeration.
			/// </summary>
			Microsoft::WRL::ComPtr<IMMDeviceEnumerator> pEnumerator;
			/// <summary>
			/// The WASAPI interface that handles the rendering.
			/// </summary>
			Microsoft::WRL::ComPtr<IAudioClient3> pRenderAudioClient;
			/// <summary>
			/// The WASAPI interface used for setting and getting the master volume.
			/// </summary>
			Microsoft::WRL::ComPtr<IAudioSessionManager2> pRenderSessionManager;
			/// <summary>
			/// The WASAPI interface that notifies the WinAudio instance when a render session event happens. 
			/// </summary>
			Microsoft::WRL::ComPtr<IAudioSessionControl> pRenderSessionControl;
			/// <summary>
			/// The WASAPI interface that handles the capturing.
			/// </summary>
			Microsoft::WRL::ComPtr<IAudioClient3> pCaptureAudioClient;
			/// <summary>
			/// The WASAPI interface used for handling capture session events.
			/// </summary>
			Microsoft::WRL::ComPtr<IAudioSessionManager2> pCaptureSessionManager;
			/// <summary>
			/// The WASAPI interface that notifies the WinAudio instance when a capture session event happens. 
			/// </summary>
			Microsoft::WRL::ComPtr<IAudioSessionControl> pCaptureSessionControl;
			/// <summary>
			/// The instance that handles the session events.
			/// </summary>
			AudioSessionEvents renderSessionEvents;
			/// <summary>
			/// The instance that handles the session events.
			/// </summary>
			AudioSessionEvents captureSessionEvents;
			/// <summary>
			/// The instance that handles the device events.
			/// </summary>
			AudioDeviceEvents deviceEvents;
		public:
			/// <summary>
			/// Creates and initalizes a WinAudio instance.
			/// </summary>
			WinAudio();
			WinAudio(const WinAudio&) = delete;
			WinAudio& operator=(const WinAudio&) = delete;
			/// <summary>
			/// Frees the WASAPI resources.
			/// </summary>
			virtual ~WinAudio();
			virtual void SetMasterVolume(hephaudio_float volume) override;
			virtual hephaudio_float GetMasterVolume() const override;
			virtual void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			virtual void StopRendering() override;
			virtual void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			virtual void StopCapturing() override;
			virtual void SetDisplayName(StringBuffer displayName) override;
			virtual void SetIconPath(StringBuffer iconPath) override;
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const override;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const override;
		protected:
			virtual void RenderData();
			virtual void CaptureData();
			static EDataFlow DeviceTypeToDataFlow(AudioDeviceType deviceType);
			static AudioDeviceType DataFlowToDeviceType(EDataFlow dataFlow);
		};
	}
}
#endif