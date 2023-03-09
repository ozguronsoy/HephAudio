#include "AudioFormatInfo.h"

namespace HephAudio
{
	AudioFormatInfo::AudioFormatInfo() : AudioFormatInfo(0, 0, 0, 0) { }
	AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t channelCount, uint16_t bps, uint32_t sampleRate)
		: formatTag(formatTag), channelCount(channelCount), bitsPerSample(bps), sampleRate(sampleRate), headerSize(0) {}
	bool AudioFormatInfo::operator==(const AudioFormatInfo& rhs) const
	{
		return this->formatTag == rhs.formatTag && this->bitsPerSample == rhs.bitsPerSample && this->channelCount == rhs.channelCount && this->sampleRate == rhs.sampleRate;
	}
	bool AudioFormatInfo::operator!=(const AudioFormatInfo& rhs) const
	{
		return this->formatTag != rhs.formatTag || this->bitsPerSample != rhs.bitsPerSample || this->channelCount != rhs.channelCount || this->sampleRate != rhs.sampleRate;
	}
	uint16_t AudioFormatInfo::FrameSize() const noexcept
	{
		return this->channelCount * this->bitsPerSample * 0.125;
	}
	uint32_t AudioFormatInfo::BitRate() const noexcept
	{
		return this->ByteRate() * 8;
	}
	uint32_t AudioFormatInfo::ByteRate() const noexcept
	{
		return this->sampleRate * this->FrameSize();
	}
#ifdef _WIN32
	AudioFormatInfo::AudioFormatInfo(const WAVEFORMATEX& wfx)
	{
		this->formatTag = wfx.wFormatTag;
		this->channelCount = wfx.nChannels;
		this->sampleRate = wfx.nSamplesPerSec;
		this->bitsPerSample = wfx.wBitsPerSample;
		this->headerSize = wfx.cbSize;
	}
	AudioFormatInfo::operator WAVEFORMATEX() const
	{
		WAVEFORMATEX wfx;
		wfx.wFormatTag = this->formatTag;
		wfx.nChannels = this->channelCount;
		wfx.nSamplesPerSec = this->sampleRate;
		wfx.nAvgBytesPerSec = this->ByteRate();
		wfx.nBlockAlign = this->FrameSize();
		wfx.wBitsPerSample = this->bitsPerSample;
		wfx.cbSize = this->headerSize;
		return wfx;
	}
#endif
}