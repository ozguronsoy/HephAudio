#pragma once
#include "framework.h"
#include "IAudioFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		class Mp3Format final : public IAudioFormat
		{
		public:
			virtual ~Mp3Format() = default;
			StringBuffer Extension() const noexcept override;
			void ReadFile(const AudioFile& file, AudioBuffer& outBuffer) const override;
			bool SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		private:
			uint32_t LocateAudioData(void* audioFileBuffer) const;
			std::vector<uint8_t> ReadAudioFrame(void* audioFileBuffer, uint32_t frameStart, uint32_t* frameLength) const;
			// In kbps
			uint16_t GetBitrate(uint8_t bitrateBits, uint8_t mpegVID) const;
			uint32_t GetSampleRate(uint8_t srBits, uint8_t mpegVID) const;
		};
	}
}