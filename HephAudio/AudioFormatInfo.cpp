#include "AudioFormatInfo.h"

namespace HephAudio
{
	namespace Structs
	{
		AudioFormatInfo::AudioFormatInfo() : AudioFormatInfo(1, 2, 16, 48000) {}
		AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t channelCount, uint16_t bps, uint32_t sampleRate)
		{
			this->formatTag = formatTag;
			this->channelCount = channelCount;
			this->sampleRate = sampleRate;
			this->bitsPerSample = bps;
			this->headerSize = 0;
		}
		bool AudioFormatInfo::operator==(const AudioFormatInfo& rhs) const
		{
			return this->formatTag == rhs.formatTag && this->bitsPerSample == rhs.bitsPerSample && this->channelCount == rhs.channelCount && this->sampleRate == rhs.sampleRate;
		}
		bool AudioFormatInfo::operator!=(const AudioFormatInfo& rhs) const
		{
			return !((*this) == rhs);
		}
		uint16_t AudioFormatInfo::FrameSize() const noexcept
		{
			return channelCount * bitsPerSample / 8;
		}
		uint32_t AudioFormatInfo::BitRate() const noexcept
		{
			return ByteRate() * 8;
		}
		uint32_t AudioFormatInfo::ByteRate() const noexcept
		{
			return sampleRate * FrameSize();
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
}