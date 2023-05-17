#pragma once
#include "HephAudioFramework.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class WavFormat final : public IAudioFileFormat
		{
		public:
			HephCommon::StringBuffer Extension() const override;
			AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File* pAudioFile) const override;
			AudioBuffer ReadFile(const HephCommon::File* pAudioFile) const override;
			bool SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		};
	}
}