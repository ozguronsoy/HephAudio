#pragma once
#if defined(HEPHAUDIO_USE_FFMPEG)
#include "HephAudioShared.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class Mp3Format final : public IAudioFileFormat
		{
		public:
			HephCommon::StringBuffer Extensions() override;
			bool VerifySignature(const HephCommon::File& audioFile) override;
			size_t FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo) override;
			AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File& audioFile) override;
			AudioBuffer ReadFile(const HephCommon::File& audioFile) override;
			AudioBuffer ReadFile(const HephCommon::File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) override;
			bool SaveToFile(const HephCommon::StringBuffer& filePath, AudioBuffer& buffer, bool overwrite) override;
		};
	}
}
#endif