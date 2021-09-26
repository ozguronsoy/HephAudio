#pragma once
#include "framework.h"
#include "IAudioFormat.h"

// https://xiph.org/flac/format.html

namespace HephAudio
{
	namespace Formats
	{
		class FlacFormat final : public IAudioFormat
		{
		public:
			virtual ~FlacFormat() = default;
			std::wstring Extension() const noexcept;
			AudioBuffer ReadFile(AudioFile& file) const;
			bool SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const;
		};
	}
}