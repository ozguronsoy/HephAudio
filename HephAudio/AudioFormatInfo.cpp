#include "AudioFormatInfo.h"

namespace HephAudio
{
	namespace Structs
	{
		AudioFormatInfo::AudioFormatInfo()
		{
			wFormatTag = 1;
			nChannels = 2;
			nSamplesPerSec = 48000;
			nAvgBytesPerSec = 192000;
			nBlockAlign = 4;
			wBitsPerSample = 16;
			headerSize = 0;
		}
		AudioFormatInfo::AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate)
		{
			this->wFormatTag = formatTag;
			this->nChannels = nChannels;
			this->nSamplesPerSec = sampleRate;
			this->wBitsPerSample = bps;
			this->nBlockAlign = nChannels * bps / 8;
			this->nAvgBytesPerSec = sampleRate * this->nBlockAlign;
			this->headerSize = 0;
		}
		bool AudioFormatInfo::operator==(const AudioFormatInfo& rhs) const
		{
			return this->wFormatTag == rhs.wFormatTag && this->wBitsPerSample == rhs.wBitsPerSample && this->nChannels == rhs.nChannels && this->nSamplesPerSec == rhs.nSamplesPerSec
				&& this->nBlockAlign == rhs.nBlockAlign && this->nAvgBytesPerSec == rhs.nAvgBytesPerSec;
		}
		bool AudioFormatInfo::operator!=(const AudioFormatInfo& rhs) const
		{
			return !((*this) == rhs);
		}
#ifdef _WIN32
		AudioFormatInfo::AudioFormatInfo(const WAVEFORMATEX& wfx)
		{
			this->wFormatTag = wfx.wFormatTag;
			this->nChannels = wfx.nChannels;
			this->nSamplesPerSec = wfx.nSamplesPerSec;
			this->nAvgBytesPerSec = wfx.nAvgBytesPerSec;
			this->nBlockAlign = wfx.nBlockAlign;
			this->wBitsPerSample = wfx.wBitsPerSample;
			this->headerSize = wfx.cbSize;
		}
		AudioFormatInfo::operator WAVEFORMATEX() const
		{
			WAVEFORMATEX wfx;
			wfx.wFormatTag = this->wFormatTag;
			wfx.nChannels = this->nChannels;
			wfx.nSamplesPerSec = this->nSamplesPerSec;
			wfx.nAvgBytesPerSec = this->nAvgBytesPerSec;
			wfx.nBlockAlign = this->nBlockAlign;
			wfx.wBitsPerSample = this->wBitsPerSample;
			wfx.cbSize = this->headerSize;
			return wfx;
		}
#endif
	}
}