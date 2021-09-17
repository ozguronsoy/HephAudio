#pragma once
#include "framework.h"
#include "INativeAudio.h"
#include <wrl.h>
#include <Mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <audioclient.h>
#include <audiopolicy.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses WASAPI. Use WinAudioDS if you have Windows XP or lower.
		class HephAudioAPI WinAudio : public INativeAudio
		{
#pragma region Audio Session Events
		protected:
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
				HRESULT STDMETHODCALLTYPE QueryInterface(REFIID  riid, VOID** ppvInterface);
				HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext);
				HRESULT STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext);
				HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext);
				HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext);
				HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext);
				HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState NewState);
				HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason);
			};
#pragma endregion
#pragma region Audio Device Events
		protected:
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
				HRESULT STDMETHODCALLTYPE QueryInterface(REFIID  riid, VOID** ppvInterface);
				HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId);
				HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
				HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
				HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
				HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);
			};
#pragma endregion
		protected:
			HANDLE hEvent;
			Microsoft::WRL::ComPtr<IMMDeviceEnumerator> pEnumerator;
			Microsoft::WRL::ComPtr<IAudioClient3> pAudioClient;
			Microsoft::WRL::ComPtr<IAudioSessionManager2> pSessionManager;
			Microsoft::WRL::ComPtr<IAudioSessionControl> pSessionControl;
			AudioSessionEvents sessionEvents;
			AudioDeviceEvents deviceEvents;
			std::wstring currentRenderDeviceId;
			std::wstring currentCaptureDeviceId;
		public:
			WinAudio();
			WinAudio(const WinAudio&) = delete;
			WinAudio& operator=(const WinAudio&) = delete;
			virtual ~WinAudio();
			virtual void SetMasterVolume(double volume) const;
			virtual double GetMasterVolume() const;
			virtual void InitializeRender(AudioDevice* device, WAVEFORMATEX format);
			virtual void StopRendering();
			virtual void InitializeCapture(AudioDevice* device, WAVEFORMATEX format);
			virtual void StopCapturing();
			virtual void SetDisplayName(std::wstring displayName);
			virtual void SetIconPath(std::wstring iconPath);
			virtual AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
			virtual std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType, bool includeInactive) const;
		protected:
			virtual UINT32 GetRenderBufferSize() const;
			virtual void RenderData();
			virtual void CaptureData(Microsoft::WRL::ComPtr<IAudioClient3> pCaptureAudioClient, Microsoft::WRL::ComPtr<IAudioSessionManager2> pSessionManager);
			static EDataFlow DeviceTypeToDataFlow(AudioDeviceType deviceType);
			static AudioDeviceType DataFlowToDeviceType(EDataFlow dataFlow);
		};
	}
}