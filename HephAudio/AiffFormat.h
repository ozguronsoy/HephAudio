#pragma once
#include "framework.h"
#include "IAudioFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		class AiffFormat final : public IAudioFormat
		{
		public:
			virtual ~AiffFormat() = default;
			std::wstring Extension() const noexcept;
			AudioFormatInfo ReadFormatInfo(const AudioFile& inFile, uint32_t& outFrameCount, Endian& outEndian) const;
			AudioBuffer ReadFile(const AudioFile& file) const;
			bool SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const;
		protected:
			void SampleRateFrom64(uint64_t srBits, AudioFormatInfo* wfx) const;
			uint64_t SampleRateTo64(const AudioFormatInfo* const& wfx) const;
		};
	}
}