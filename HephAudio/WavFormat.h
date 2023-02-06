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
			virtual ~WavFormat() = default;
			StringBuffer Extension() const noexcept;
			AudioFormatInfo ReadFormatInfo(const AudioFile& file, size_t& audioDataSize) const;
			AudioBuffer ReadFile(const AudioFile& file) const;
			bool SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const;
		};
	}
}