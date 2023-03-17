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
			StringBuffer Extension() const noexcept override;
			void ReadFile(const AudioFile& file, AudioBuffer& outBuffer) const override;
			bool SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const override;
		private:
			AudioFormatInfo ReadFormatInfo(const AudioFile& file, size_t& wavHeaderSize, size_t& audioDataSize) const;
		};
	}
}