#pragma once
#ifdef _WIN32
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "WinAudioBase.h"
#include "Params/WînAudioParams.h"
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
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;
		private:
			WinAudioParams params;
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
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;
			void SetDisplayName(std::string displayName);
			void SetIconPath(std::string iconPath);
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