#pragma once
#include "HephAudioFramework.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class AiffFormat final : public IAudioFileFormat
		{
		public:
			HephCommon::StringBuffer Extensions() const override;
			bool VerifySignature(const HephCommon::File& audioFile) const override;
			size_t FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo) const override;
			AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File& audioFile) const override;
			AudioBuffer ReadFile(const HephCommon::File& audioFile) const override;
			AudioBuffer ReadFile(const HephCommon::File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) const override;
			bool SaveToFile(const HephCommon::StringBuffer& filePath, AudioBuffer& buffer, bool overwrite) const override;
		private:
			void SampleRateFrom64(uint64_t srBits, AudioFormatInfo& formatInfo) const;
			uint64_t SampleRateTo64(const AudioFormatInfo& formatInfo) const;
			void FormatTagFrom32(uint32_t tagBits, AudioFormatInfo& formatInfo) const;
			void FormatTagTo32(const AudioFormatInfo& audioFormatInfo, uint32_t& outTagBits, HephCommon::StringBuffer& outCompressionName) const;
		};
	}
}