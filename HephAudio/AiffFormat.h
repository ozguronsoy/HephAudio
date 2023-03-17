#pragma once
#include "framework.h"
#include "IAudioFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		/*class AiffFormat final : public IAudioFormat
		{
		public:
			virtual ~AiffFormat() = default;
			StringBuffer Extension() const noexcept override;
			void ReadFile(const AudioFile& file, AudioBuffer& outBuffer) const override;
			bool SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		private:
			AudioFormatInfo ReadFormatInfo(const AudioFile& inFile, size_t& outWavHeaderSize, uint32_t& outFrameCount, Endian& outEndian) const;
			void SampleRateFrom64(uint64_t srBits, AudioFormatInfo* wfx) const;
			uint64_t SampleRateTo64(const AudioFormatInfo* const& wfx) const;
		};*/
	}
}