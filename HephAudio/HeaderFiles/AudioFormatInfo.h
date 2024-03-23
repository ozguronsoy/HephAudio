#pragma once
#include "HephAudioShared.h"
#include <cstdint>

#define HEPHAUDIO_INTERNAL_FORMAT(channelCount, sampleRate) AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL, channelCount, sizeof(heph_audio_sample) * 8, sampleRate, HEPH_SYSTEM_ENDIAN)

namespace HephAudio
{
	struct AudioFormatInfo
	{
		uint16_t formatTag;
		uint16_t channelCount;
		uint16_t bitsPerSample;
		uint32_t sampleRate;
		uint32_t bitRate;
		HephCommon::Endian endian;
		AudioFormatInfo();
		AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bitsPerSample, uint32_t sampleRate);
		AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bitsPerSample, uint32_t sampleRate, uint32_t bitRate);
		AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bitsPerSample, uint32_t sampleRate, HephCommon::Endian endian);
		AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bitsPerSample, uint32_t sampleRate, uint32_t bitRate, HephCommon::Endian endian);
		bool operator==(const AudioFormatInfo& rhs) const;
		bool operator!=(const AudioFormatInfo& rhs) const;
		uint16_t FrameSize() const;
		uint32_t ByteRate() const;
		static uint32_t CalculateBitrate(const AudioFormatInfo& formatInfo);
	};
}