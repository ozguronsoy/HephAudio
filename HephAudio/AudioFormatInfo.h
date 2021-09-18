#pragma once
#include "framework.h"
#include <cstdint>
#ifdef _WIN32
#include <Mmreg.h>
#include <mmeapi.h>
#endif

namespace HephAudio
{
	namespace Structs
	{
		struct HephAudioAPI AudioFormatInfo
		{

			uint16_t wFormatTag;        
			uint16_t nChannels;
			uint32_t nSamplesPerSec;
			uint32_t nAvgBytesPerSec;
			uint16_t nBlockAlign;
			uint16_t wBitsPerSample;
			uint16_t headerSize;
			AudioFormatInfo();
			AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate);
			virtual ~AudioFormatInfo() = default;
			bool operator==(const AudioFormatInfo& rhs) const;
			bool operator!=(const AudioFormatInfo& rhs) const;
#ifdef _WIN32
			AudioFormatInfo(const WAVEFORMATEX& wfx);
			operator WAVEFORMATEX() const;
#endif
		};
	}
}