#pragma once
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers 

#include "HephAudioFramework.h"
#include "NativeAudio.h"
#include <windows.h>
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
		class WinAudio final : public NativeAudio
		{
		private:
			HANDLE hEvent;
			Microsoft::WRL::ComPtr<IMMDeviceEnumerator> pEnumerator;
			Microsoft::WRL::ComPtr<IAudioClient> pRenderAudioClient;
			Microsoft::WRL::ComPtr<IAudioSessionManager> pRenderSessionManager;
			Microsoft::WRL::ComPtr<IAudioSessionControl> pRenderSessionControl;
			Microsoft::WRL::ComPtr<IAudioClient> pCaptureAudioClient;
		public:
			WinAudio();
			WinAudio(const WinAudio&) = delete;
			WinAudio& operator=(const WinAudio&) = delete;
			~WinAudio();
			void SetMasterVolume(heph_float volume) override;
			heph_float GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void SetDisplayName(HephCommon::StringBuffer displayName) override;
			void SetIconPath(HephCommon::StringBuffer iconPath) override;
		private:
			bool EnumerateAudioDevices() override;
			void RenderData();
			void CaptureData();
			static EDataFlow DeviceTypeToDataFlow(AudioDeviceType deviceType);
			static AudioDeviceType DataFlowToDeviceType(EDataFlow dataFlow);
			static AudioFormatInfo WFX2AFI(const WAVEFORMATEX& wfx) noexcept;
			static WAVEFORMATEX AFI2WFX(const AudioFormatInfo& afi) noexcept;
		};
	}
}
#endif