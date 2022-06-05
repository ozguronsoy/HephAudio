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
		struct AudioFormatInfo
		{
			uint16_t formatTag;        
			uint16_t channelCount;
			uint32_t sampleRate;
			uint16_t bitsPerSample;
			uint16_t headerSize;
			AudioFormatInfo();
			AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate);
			virtual ~AudioFormatInfo() = default;
			bool operator==(const AudioFormatInfo& rhs) const;
			bool operator!=(const AudioFormatInfo& rhs) const;
			uint16_t FrameSize() const noexcept;
			uint32_t BitRate() const noexcept;
			uint32_t ByteRate() const noexcept;
#ifdef _WIN32
			AudioFormatInfo(const WAVEFORMATEX& wfx);
			operator WAVEFORMATEX() const;
#endif
		};
	}
}