#include "WavFileMetadataReader.h"
#include "HephException.h"

namespace HephAudio
{
	namespace FileFormats
	{
		namespace MetadataReaders
		{
			void WavFileMetadataReader::Read(const HephCommon::File* pFile, WavFmtChunk* pFmtChunk, WavFactChunk* pFactChunk)
			{
				pFile->SetOffset(16);

				uint32_t chunkSize;
				if (pFmtChunk != nullptr)
				{
					pFile->Read(&chunkSize, 4, Endian::Little);
					pFile->Read(&pFmtChunk->wFormatTag, 2, Endian::Little);
					pFile->Read(&pFmtChunk->nChannels, 2, Endian::Little);
					pFile->Read(&pFmtChunk->nSamplesPerSec, 4, Endian::Little);
					pFile->Read(&pFmtChunk->nAvgBytesPerSec, 4, Endian::Little);
					pFile->Read(&pFmtChunk->nBlockAlign, 2, Endian::Little);
					pFile->Read(&pFmtChunk->wBitsPerSample, 2, Endian::Little);

					if (pFmtChunk->wFormatTag != WAVE_FORMAT_PCM)
					{
						pFile->Read(&pFmtChunk->cbSize, 2, Endian::Little);
						pFile->Read(&pFmtChunk->wValidBitsPerSample, 2, Endian::Little);
						pFile->Read(&pFmtChunk->dwChannelMask, 4, Endian::Little);
						pFile->Read(&pFmtChunk->subFormat, 12, Endian::Little);
					}
				}
				else // skip the fmt chunk
				{
					pFile->Read(&chunkSize, 4, Endian::Little);
				}
				pFile->SetOffset(chunkSize + 20);

				if (pFactChunk != nullptr)
				{
					constexpr uint32_t factID = 0x66616374;

					uint32_t chunkID;
					pFile->Read(&chunkID, 4, Endian::Big);
					if (chunkID == factID)
					{
						pFile->Read(&chunkSize, 4, Endian::Little);
						pFile->Read(&pFactChunk->dwSampleLength, chunkSize, Endian::Little);
					}
				}
			}
		}
	}
}