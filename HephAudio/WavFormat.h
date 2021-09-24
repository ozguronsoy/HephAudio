#pragma once
#include "IAudioFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		class WavFormat final : public IAudioFormat
		{
		public:
			virtual ~WavFormat() = default;
			std::wstring Extension() const noexcept;
			AudioFormatInfo ReadFormatInfo(AudioFile& file) const;
			AudioBuffer ReadFile(AudioFile& file) const;
			bool SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const;
		};
	}
}