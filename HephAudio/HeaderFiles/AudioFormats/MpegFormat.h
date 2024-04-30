#pragma once
#include "HephAudioShared.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class MpegFormat final : public IAudioFileFormat
		{
		public:
			std::string Extensions() override;
			bool VerifySignature(const HephCommon::File& audioFile) override;
			size_t FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo) override;
			AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File& audioFile) override;
			AudioBuffer ReadFile(const HephCommon::File& audioFile) override;
			AudioBuffer ReadFile(const HephCommon::File& audioFile, size_t frameIndex, size_t frameCount) override;
			bool SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite) override;
		};
	}
}