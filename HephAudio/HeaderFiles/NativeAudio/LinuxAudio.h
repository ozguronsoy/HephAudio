#pragma once
#if defined(__linux__) && !defined(__ANDROID__)
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "AudioEvents/AudioDeviceEventArgs.h"
#include "AudioEvents/AudioCaptureEventArgs.h"
#include "Params/AlsaParams.h"
#include <alsa/asoundlib.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief uses ALSA
		 * 
		 */
		class HEPH_API LinuxAudio final : public NativeAudio
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;

		private:
			static constexpr long volume_max = INT32_MAX;

		private:
			AlsaParams params;
			snd_pcm_t* renderPcm;
			snd_pcm_t* capturePcm;

		public:
			/** @copydoc default_constructor */
			LinuxAudio();

			LinuxAudio(const LinuxAudio&) = delete;
			LinuxAudio& operator=(const LinuxAudio&) = delete;

			/** @copydoc destructor */
			~LinuxAudio();

			void SetMasterVolume(double volume) override;
			double GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;

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