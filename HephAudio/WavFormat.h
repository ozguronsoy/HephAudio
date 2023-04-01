#pragma once
#include "framework.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class WavFormat final : public IAudioFileFormat
		{
		public:
			HephCommon::StringBuffer Extension() const override;
			AudioFormatInfo ReadAudioFormatInfo(const AudioFile* pAudioFile) const override;
			AudioBuffer ReadFile(const AudioFile* pAudioFile) const override;
			bool SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		};
	}
}