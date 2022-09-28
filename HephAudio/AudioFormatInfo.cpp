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
#pragma region Exports
#if defined(_WIN32)
AudioFormatInfo* _stdcall CreateFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate)
{
	return new HephAudio::Structs::AudioFormatInfo(formatTag, nChannels, bps, sampleRate);
}
uint16_t _stdcall FormatInfoGetFormatTag(AudioFormatInfo* pFormatInfo)
{
	return pFormatInfo->formatTag;
}
void _stdcall FormatInfoSetFormatTag(AudioFormatInfo* pFormatInfo, uint16_t formatTag)
{
	pFormatInfo->formatTag = formatTag;
}
uint16_t _stdcall FormatInfoGetChannelCount(AudioFormatInfo* pFormatInfo)
{
	return pFormatInfo->channelCount;
}
void _stdcall FormatInfoSetChannelCount(AudioFormatInfo* pFormatInfo, uint16_t channelCount)
{
	pFormatInfo->channelCount = channelCount;
}
uint32_t _stdcall FormatInfoGetSampleRate(AudioFormatInfo* pFormatInfo)
{
	return pFormatInfo->sampleRate;
}
void _stdcall FormatInfoSetSampleRate(AudioFormatInfo* pFormatInfo, uint32_t sampleRate)
{
	pFormatInfo->sampleRate = sampleRate;
}
uint16_t _stdcall FormatInfoGetBitsPerSample(AudioFormatInfo* pFormatInfo)
{
	return pFormatInfo->bitsPerSample;
}
void _stdcall FormatInfoSetBitsPerSample(AudioFormatInfo* pFormatInfo, uint16_t bitsPerSample)
{
	pFormatInfo->bitsPerSample = bitsPerSample;
}
void _stdcall DestroyFormatInfo(AudioFormatInfo* pFormatInfo)
{
	delete pFormatInfo;
}
#endif
#pragma endregion