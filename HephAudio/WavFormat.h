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
			AudioFormatInfo ReadFormatInfo(const AudioFile& file, size_t& audioDataSize) const;
			AudioBuffer ReadFile(const AudioFile& file) const;
			bool SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const;
		};
	}
}