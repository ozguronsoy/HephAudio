#pragma once
#if defined(__linux__)
#include "HephAudioFramework.h"
#include "NativeAudio.h"
#include <alsa/asoundlib.h>

namespace HephAudio
{
	namespace Native
	{
		/// <summary>
		/// Uses ALSA. Add -lasound to the compiler flags.
		/// </summary>
		class LinuxAudio final : public NativeAudio
		{
		private:
			static constexpr long volume_max = INT32_MAX;
		private:
			snd_pcm_t* renderPcm;
			snd_pcm_t* capturePcm;
		public:
			LinuxAudio();
			LinuxAudio(const LinuxAudio&) = delete;
			LinuxAudio& operator=(const LinuxAudio&) = delete;
			~LinuxAudio();
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
			snd_pcm_format_t ToPcmFormat(const AudioFormatInfo& format) const noexcept;
		};
	}
}
#endif