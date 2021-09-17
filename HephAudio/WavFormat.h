#pragma once
#include "IAudioFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		class HephAudioAPI WavFormat final : public IAudioFormat
		{
		public:
			virtual ~WavFormat() = default;
			std::wstring Extension() const noexcept;
			WAVEFORMATEX ReadFormatInfo(AudioFile& file) const;
			AudioBuffer ReadFile(AudioFile& file) const;
			bool SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const;
		};
	}
}