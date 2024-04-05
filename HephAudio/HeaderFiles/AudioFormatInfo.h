#pragma once
#include "HephAudioShared.h"
#include "AudioChannelLayout.h"

#define HEPHAUDIO_INTERNAL_FORMAT(chLayout, sampleRate) HephAudio::AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL, sizeof(heph_audio_sample) * 8, chLayout, sampleRate, HEPH_SYSTEM_ENDIAN)

namespace HephAudio
{
	struct AudioFormatInfo
	{
		uint16_t formatTag;
		uint16_t bitsPerSample;
		AudioChannelLayout channelLayout;
		uint32_t sampleRate;
		uint32_t bitRate;
		HephCommon::Endian endian;
		AudioFormatInfo();
		AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate);
		AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, uint32_t bitRate);
		AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, HephCommon::Endian endian);
		AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, uint32_t bitRate, HephCommon::Endian endian);
		bool operator==(const AudioFormatInfo& rhs) const;
		bool operator!=(const AudioFormatInfo& rhs) const;
		uint16_t FrameSize() const;
		uint32_t ByteRate() const;
		static uint32_t CalculateBitrate(const AudioFormatInfo& formatInfo);
	};
}