#include "AudioFormatInfo.h"

namespace HephAudio
{
	AudioFormatInfo::AudioFormatInfo() : AudioFormatInfo(0, 0, 0, 0, HEPH_SYSTEM_ENDIAN) { }
	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t channelCount, uint16_t bps, uint32_t sampleRate) 
		: AudioFormatInfo(formatTag, channelCount, bps, sampleRate, HEPH_SYSTEM_ENDIAN) {}
	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t channelCount, uint16_t bps, uint32_t sampleRate, HephCommon::Endian endian)
		: formatTag(formatTag), channelCount(channelCount), bitsPerSample(bps), sampleRate(sampleRate), endian(endian) {}
	bool AudioFormatInfo::operator==(const AudioFormatInfo& rhs) const
	{
		return this->formatTag == rhs.formatTag && this->bitsPerSample == rhs.bitsPerSample && this->channelCount == rhs.channelCount && this->sampleRate == rhs.sampleRate && this->endian == rhs.endian;
	}
	bool AudioFormatInfo::operator!=(const AudioFormatInfo& rhs) const
	{
		return this->formatTag != rhs.formatTag || this->bitsPerSample != rhs.bitsPerSample || this->channelCount != rhs.channelCount || this->sampleRate != rhs.sampleRate || this->endian != rhs.endian;
	}
	uint16_t AudioFormatInfo::FrameSize() const noexcept
	{
		return this->channelCount * this->bitsPerSample * 0.125;
	}
	uint32_t AudioFormatInfo::BitRate() const noexcept
	{
		return this->sampleRate * this->channelCount * this->bitsPerSample;
	}
	uint32_t AudioFormatInfo::ByteRate() const noexcept
	{
		return this->sampleRate * this->channelCount * this->bitsPerSample * 0.125;
	}
}