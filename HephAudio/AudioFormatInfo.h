#pragma once
#include "HephAudioFramework.h"
#include <cstdint>
#if defined(_WIN32)
#include <Mmreg.h>
#include <mmeapi.h>
#endif

#define HEPHAUDIO_INTERNAL_FORMAT(channelCount, sampleRate) AudioFormatInfo(WAVE_FORMAT_IEEE_FLOAT, channelCount, sizeof(heph_float) * 8, sampleRate, HEPH_SYSTEM_ENDIAN)

namespace HephAudio
{
	struct AudioFormatInfo
	{
		uint16_t formatTag;
		uint16_t channelCount;
		uint32_t sampleRate;
		uint16_t bitsPerSample;
		HephCommon::Endian endian;
		AudioFormatInfo();
		AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate);
		AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate, HephCommon::Endian endian);
		bool operator==(const AudioFormatInfo& rhs) const;
		bool operator!=(const AudioFormatInfo& rhs) const;
		uint16_t FrameSize() const noexcept;
		uint32_t BitRate() const noexcept;
		uint32_t ByteRate() const noexcept;
#if defined(_WIN32)
		AudioFormatInfo(const WAVEFORMATEX& wfx);
		operator WAVEFORMATEX() const noexcept;
#endif
	};
}