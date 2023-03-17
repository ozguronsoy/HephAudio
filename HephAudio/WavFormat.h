#pragma once
#include "framework.h"
#include "IAudioFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		class WavFormat final : public IAudioFormat
		{
		public:
			StringBuffer Extension() const noexcept override;
			AudioFormatInfo ReadAudioFormatInfo(const AudioFile* pAudioFile) const noexcept override;
			AudioBuffer ReadFile(const AudioFile* pAudioFile) const override;
			bool SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		};
	}
}