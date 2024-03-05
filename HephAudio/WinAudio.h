#pragma once
#ifdef _WIN32
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "WinAudioBase.h"
#include <wrl.h>
#include <Mmdeviceapi.h>
#include <audiopolicy.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses WASAPI
		class WinAudio final : public WinAudioBase
		{
		private:
			Microsoft::WRL::ComPtr<IMMDeviceEnumerator> pEnumerator;
			Microsoft::WRL::ComPtr<IAudioSessionManager> pRenderSessionManager;
			Microsoft::WRL::ComPtr<IAudioSessionControl> pRenderSessionControl;
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
			void SetDisplayName(HephCommon::StringBuffer displayName);
			void SetIconPath(HephCommon::StringBuffer iconPath);
		private:
			bool EnumerateAudioDevices() override;
			void CheckAudioDevices() override;
			void RenderData(AudioDevice* device, AudioFormatInfo format);
			void CaptureData(AudioDevice* device, AudioFormatInfo format);
			static AudioDeviceType DataFlowToDeviceType(EDataFlow dataFlow);
		};
	}
}
#endif