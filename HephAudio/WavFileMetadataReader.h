#pragma once
#include "HephAudioFramework.h"
#include "File.h"

namespace HephAudio
{
	namespace FileFormats
	{
		namespace MetadataReaders
		{
			struct WavFmtChunk
			{
				uint16_t wFormatTag{ 0 };
				uint16_t nChannels{ 0 };
				uint32_t nSamplesPerSec{ 0 };
				uint32_t nAvgBytesPerSec{ 0 };
				uint16_t nBlockAlign{ 0 };
				uint16_t wBitsPerSample{ 0 };
				uint16_t cbSize{ 0 };
				uint16_t wValidBitsPerSample{ 0 };
				uint32_t dwChannelMask{ 0 };
				GUID subFormat{ 0 };
			};
			struct WavFactChunk
			{
				uint64_t dwSampleLength{ 0 };
			};
			class WavFileMetadataReader final
			{
			public:
				WavFileMetadataReader() = delete;
				WavFileMetadataReader(const WavFileMetadataReader&) = delete;
				WavFileMetadataReader& operator=(const WavFileMetadataReader&) = delete;
				static void Read(const HephCommon::File* pFile, WavFmtChunk* pFmtChunk, WavFactChunk* pFactChunk);
			};
		}
	}
}