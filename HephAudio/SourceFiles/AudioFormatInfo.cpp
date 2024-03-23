#include "AudioFormatInfo.h"

namespace HephAudio
{
	AudioFormatInfo::AudioFormatInfo() : AudioFormatInfo(0, 0, 0, 0, HEPH_SYSTEM_ENDIAN) { }

	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t channelCount, uint16_t bitsPerSample, uint32_t sampleRate)
		: AudioFormatInfo(formatTag, channelCount, bitsPerSample, sampleRate, 128000, HEPH_SYSTEM_ENDIAN) {}

	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t channelCount, uint16_t bitsPerSample, uint32_t sampleRate, uint32_t bitRate)
		: AudioFormatInfo(formatTag, channelCount, bitsPerSample, sampleRate, bitRate, HEPH_SYSTEM_ENDIAN) {}

	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t channelCount, uint16_t bitsPerSample, uint32_t sampleRate, HephCommon::Endian endian)
		: AudioFormatInfo(formatTag, channelCount, bitsPerSample, sampleRate, 128000, endian) {}

	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t channelCount, uint16_t bitsPerSample, uint32_t sampleRate, uint32_t bitRate, HephCommon::Endian endian)
		: formatTag(formatTag), channelCount(channelCount), bitsPerSample(bitsPerSample)
		, sampleRate(sampleRate), bitRate(bitRate), endian(endian)
	{
		this->bitRate = AudioFormatInfo::CalculateBitrate(*this);
	}

	bool AudioFormatInfo::operator==(const AudioFormatInfo& rhs) const
	{
		return this->formatTag == rhs.formatTag && this->bitsPerSample == rhs.bitsPerSample
			&& this->channelCount == rhs.channelCount && this->sampleRate == rhs.sampleRate
			&& this->bitRate == rhs.bitRate && this->endian == rhs.endian;
	}
	bool AudioFormatInfo::operator!=(const AudioFormatInfo& rhs) const
	{
		return this->formatTag != rhs.formatTag || this->bitsPerSample != rhs.bitsPerSample
			|| this->channelCount != rhs.channelCount || this->sampleRate != rhs.sampleRate
			|| this->bitRate != rhs.bitRate || this->endian != rhs.endian;
	}
	uint16_t AudioFormatInfo::FrameSize() const
	{
		return this->channelCount * this->bitsPerSample / 8;
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
			return formatInfo.channelCount * formatInfo.bitsPerSample * formatInfo.sampleRate;
		}
		return formatInfo.bitRate;
	}
}