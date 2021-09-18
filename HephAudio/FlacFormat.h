#pragma once
#include "framework.h"
#include "IAudioFormat.h"

// https://xiph.org/flac/format.html

namespace HephAudio
{
	namespace Formats
	{
		class HephAudioAPI FlacFormat final : public IAudioFormat
		{
		public:
			virtual ~FlacFormat() = default;
			std::wstring Extension() const noexcept;
			AudioFormatInfo ReadFormatInfo(AudioFile& file) const;
			AudioBuffer ReadFile(AudioFile& file) const;
			bool SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const;
		private:
			void ReadHeader(void* audioFileBuffer, uint32_t startOfHeader, AudioFormatInfo& wfx) const;
			uint16_t GetBlockSize(void* audioFileBuffer, uint32_t startOfHeader, uint8_t bsbits, bool variableBlocking) const;
			uint16_t GetBPS(uint8_t bpsbits, AudioFormatInfo& wfx) const;
		};
	}
}