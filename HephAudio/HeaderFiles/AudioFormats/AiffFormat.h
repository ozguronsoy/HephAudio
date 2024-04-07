#pragma once
#include "HephAudioShared.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class AiffFormat final : public IAudioFileFormat
		{
		public:
			std::string Extensions() override;
			bool VerifySignature(const HephCommon::File& audioFile) override;
			size_t FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo) override;
			AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File& audioFile) override;
			AudioBuffer ReadFile(const HephCommon::File& audioFile) override;
			AudioBuffer ReadFile(const HephCommon::File& audioFile, Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) override;
			bool SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite) override;
		private:
			void SampleRateFrom64(uint64_t srBits, AudioFormatInfo& formatInfo) const;
			uint64_t SampleRateTo64(const AudioFormatInfo& formatInfo) const;
			void FormatTagFrom32(uint32_t tagBits, AudioFormatInfo& formatInfo) const;
			void FormatTagTo32(const AudioFormatInfo& audioFormatInfo, uint32_t& outTagBits, std::string& outCompressionName) const;
		};
	}
}