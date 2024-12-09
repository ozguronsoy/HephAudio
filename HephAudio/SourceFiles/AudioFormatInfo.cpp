#include "AudioFormatInfo.h"

namespace HephAudio
{
	AudioFormatInfo::AudioFormatInfo() : AudioFormatInfo(0, 0, AudioChannelLayout(), 0, HEPH_SYSTEM_ENDIAN) { }

	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate)
		: AudioFormatInfo(formatTag, bitsPerSample, chLayout, sampleRate, 128000, HEPH_SYSTEM_ENDIAN) {}

	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, uint32_t bitRate)
		: AudioFormatInfo(formatTag, bitsPerSample, chLayout, sampleRate, bitRate, HEPH_SYSTEM_ENDIAN) {}

	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, Heph::Endian endian)
		: AudioFormatInfo(formatTag, bitsPerSample, chLayout, sampleRate, 128000, endian) {}

	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, uint32_t bitRate, Heph::Endian endian)
		: formatTag(formatTag), bitsPerSample(bitsPerSample), channelLayout(chLayout)
		, sampleRate(sampleRate), bitRate(bitRate), endian(endian)
	{
		this->bitRate = AudioFormatInfo::CalculateBitrate(*this);
	}

	bool AudioFormatInfo::operator==(const AudioFormatInfo& rhs) const
	{
		return this->formatTag == rhs.formatTag && this->bitsPerSample == rhs.bitsPerSample
			&& this->channelLayout == rhs.channelLayout && this->sampleRate == rhs.sampleRate
			&& this->bitRate == rhs.bitRate && this->endian == rhs.endian;
	}

	bool AudioFormatInfo::operator!=(const AudioFormatInfo& rhs) const
	{
		return !((*this) == rhs);
	}

	uint16_t AudioFormatInfo::FrameSize() const
	{
		return this->channelLayout.count * this->bitsPerSample / 8;
	}

	uint32_t AudioFormatInfo::ByteRate() const
	{
		return this->bitRate / 8;
	}

	uint32_t AudioFormatInfo::CalculateBitrate(const AudioFormatInfo& formatInfo)
	{
		if (formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_PCM ||
			formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT ||
			formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_ALAW ||
			formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_MULAW)
		{
			return formatInfo.channelLayout.count * formatInfo.bitsPerSample * formatInfo.sampleRate;
		}
		return formatInfo.bitRate;
	}
}