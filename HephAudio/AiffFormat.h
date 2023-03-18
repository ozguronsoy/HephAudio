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
			StringBuffer Extension() const override;
			AudioFormatInfo ReadAudioFormatInfo(const AudioFile* pAudioFile) const override;
			AudioBuffer ReadFile(const AudioFile* pAudioFile) const override;
			bool SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		private:
			void SampleRateFrom64(uint64_t srBits, AudioFormatInfo* wfx) const;
			uint64_t SampleRateTo64(const AudioFormatInfo* const& wfx) const;
		};
	}
}