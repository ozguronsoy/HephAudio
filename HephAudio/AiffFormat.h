#pragma once
#include "framework.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class AiffFormat final : public IAudioFileFormat
		{
		public:
			HephCommon::StringBuffer Extension() const override;
			AudioFormatInfo ReadAudioFormatInfo(const AudioFile* pAudioFile) const override;
			AudioBuffer ReadFile(const AudioFile* pAudioFile) const override;
			bool SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		private:
			void SampleRateFrom64(uint64_t srBits, AudioFormatInfo& formatInfo) const;
			uint64_t SampleRateTo64(const AudioFormatInfo& formatInfo) const;
			void FormatTagFrom32(uint32_t tagBits, AudioFormatInfo& formatInfo) const;
			void FormatTagTo32(const AudioFormatInfo& audioFormatInfo, uint32_t& outTagBits, HephCommon::StringBuffer& outCompressionName) const;
		};
	}
}