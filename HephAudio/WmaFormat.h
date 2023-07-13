#pragma once
#include "HephAudioFramework.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class WmaFormat final : public IAudioFileFormat
		{
		public:
			HephCommon::StringBuffer Extension() const override;
			size_t FileFrameCount(const HephCommon::File* pAudioFile, const AudioFormatInfo& audioFormatInfo) const override;
			AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File* pAudioFile) const override;
			AudioBuffer ReadFile(const HephCommon::File* pAudioFile) const override;
			AudioBuffer ReadFile(const HephCommon::File* pAudioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) const override;
			bool SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		private:
			static bool CheckGuid(const HephCommon::File* pAudioFile, uint32_t d0, uint16_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint32_t d5);
		};
	}
}