#pragma once
#if defined(__linux__) && !defined(__ANDROID__)
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include <alsa/asoundlib.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses ALSA
		class LinuxAudio final : public NativeAudio
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;
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
		private:
			bool EnumerateAudioDevices() override;
			void RenderData(useconds_t bufferDuration_us);
			void CaptureData(useconds_t bufferDuration_us);
			snd_pcm_format_t ToPcmFormat(const AudioFormatInfo& format) const;
			snd_pcm_chmap* ToPcmChmap(const AudioFormatInfo& format) const;
		};
	}
}
#endif