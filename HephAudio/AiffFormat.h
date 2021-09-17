#pragma once
#include "framework.h"
#include "IAudioFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		class HephAudioAPI AiffFormat final : public IAudioFormat
		{
		public:
			virtual ~AiffFormat() = default;
			std::wstring Extension() const noexcept;
			WAVEFORMATEX ReadFormatInfo(AudioFile& inFile, uint32_t& outFrameCount, Endian& outEndian) const;
			AudioBuffer ReadFile(AudioFile& file) const;
			bool SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const;
		protected:
			void SampleRateFrom64(uint64_t srBits, WAVEFORMATEX* wfx) const;
			uint64_t SampleRateTo64(WAVEFORMATEX* wfx) const;
		};
	}
}