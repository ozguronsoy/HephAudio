#include "WavFormat.h"
#include "AudioException.h"

namespace HephAudio
{
	namespace Formats
	{
		std::wstring WavFormat::Extension() const noexcept
		{
			return L".wav";
		}
		WAVEFORMATEX WavFormat::ReadFormatInfo(AudioFile& file) const
		{
			void* audioFileBuffer = file.GetInnerBufferAddress();
			uint32_t subChunkSize, nextChunk, waveEnd;
			WAVEFORMATEX wfx = WAVEFORMATEX();
			if (Read<uint32_t>(audioFileBuffer, 0, GetSystemEndian()) == *(uint32_t*)"RIFF")
			{
				if (Read<uint32_t>(audioFileBuffer, 8, GetSystemEndian()) == *(uint32_t*)"WAVE")
				{
					wfx.wFormatTag = Read<uint16_t>(audioFileBuffer, 20, Endian::Little);
					if (wfx.wFormatTag == WAVE_FORMAT_PCM || wfx.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
					{
						subChunkSize = Read<uint32_t>(audioFileBuffer, 16, Endian::Little);
						nextChunk = subChunkSize + 20;
						if (Read<uint32_t>(audioFileBuffer, nextChunk, GetSystemEndian()) == *(uint32_t*)"fact")
						{
							nextChunk += 12; // ignore fact chunk
						}
						wfx.nChannels = Read<uint16_t>(audioFileBuffer, 22, Endian::Little);
						wfx.nSamplesPerSec = Read<uint32_t>(audioFileBuffer, 24, Endian::Little);
						wfx.nAvgBytesPerSec = Read<uint32_t>(audioFileBuffer, 28, Endian::Little);
						wfx.nBlockAlign = Read<uint16_t>(audioFileBuffer, 32, Endian::Little);
						wfx.wBitsPerSample = Read<uint16_t>(audioFileBuffer, 34, Endian::Little);
						wfx.cbSize = nextChunk + 8; // use cbSize as headerSize.
					}
				}
				else
				{
					throw AudioException(E_FAIL, L"WavFormat", L"Failed to read the wav file. File might be corrupted.");
				}
			}
			else
			{
				throw AudioException(E_FAIL, L"WavFormat", L"Failed to read the wav file. File might be corrupted.");
			}
			return wfx;
		}
		AudioBuffer WavFormat::ReadFile(AudioFile& file) const
		{
			WAVEFORMATEX waveFormat = ReadFormatInfo(file);
			size_t audioDataSize = file.Size() - waveFormat.cbSize;
			size_t frameCount = (audioDataSize) / waveFormat.nBlockAlign;
			AudioBuffer resultBuffer(frameCount, waveFormat);
			memcpy(resultBuffer.GetInnerBufferAddress(), (uint8_t*)file.GetInnerBufferAddress() + waveFormat.cbSize, audioDataSize);
			if (GetSystemEndian() == Endian::Big && waveFormat.wBitsPerSample != 8) // switch bytes.
			{
				uint8_t* innerBuffer = (uint8_t*)resultBuffer.GetInnerBufferAddress();
				const uint32_t sampleSize = waveFormat.wBitsPerSample / 8;
				for (size_t i = 0; i < resultBuffer.Size(); i += sampleSize)
				{
					switch (sampleSize)
					{
					case 2:
					{
						uint16_t sample = Read<uint16_t>(innerBuffer, i, Endian::Little);
						memcpy(innerBuffer + i, &sample, 2);
					}
					break;
					case 3:
					{
						uint32_t sample = Read<uint32_t>(innerBuffer, i, Endian::Little) >> 8;
						memcpy(innerBuffer + i, &sample, 3);
					}
					break;
					case 4:
					{
						uint32_t sample = Read<uint32_t>(innerBuffer, i, Endian::Little);
						memcpy(innerBuffer + i, &sample, 4);
					}
					break;
					default:
						throw AudioException(E_FAIL, L"WavFormat::ReadFile", L"Invalid bps.");
					}
				}
			}
			return resultBuffer;
		}
		bool WavFormat::SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const
		{
			if (!overwrite && AudioFile::FileExists(filePath))
			{
				return false;
			}
			std::shared_ptr<AudioFile> newFile = AudioFile::CreateNew(filePath, overwrite);
			if (newFile == nullptr)
			{
				return false;
			}
			const size_t headerSize = 44;
			const uint32_t dataSize = buffer.Size();
			const size_t fullBufferSize = dataSize + headerSize;
			uint8_t* newBuffer = (uint8_t*)malloc(fullBufferSize);
			if (newBuffer != nullptr)
			{
				const uint8_t riff[4] = { 'R', 'I', 'F', 'F' };
				const uint32_t chunkSize = ChangeEndian32(4, Endian::Little);
				const uint8_t riffType[4] = { 'W', 'A', 'V', 'E' };
				const uint8_t fmt[4] = { 'f', 'm', 't', ' ' };
				const uint32_t subChunkSize = ChangeEndian32(16, Endian::Little);;
				const uint8_t d[4] = { 'd', 'a', 't', 'a' };
				const uint32_t dataSizeL = ChangeEndian32(dataSize, Endian::Little);
				WAVEFORMATEX wfx = buffer.GetFormat();
				const uint16_t formatTag = ChangeEndian16(wfx.wFormatTag, Endian::Little);
				const uint16_t nChannels = ChangeEndian16(wfx.nChannels, Endian::Little);
				const uint32_t sampleRate = ChangeEndian32(wfx.nSamplesPerSec, Endian::Little);
				const uint32_t byteRate = ChangeEndian32(wfx.nAvgBytesPerSec, Endian::Little);
				const uint16_t blockAlign = ChangeEndian16(wfx.nBlockAlign, Endian::Little);
				const uint16_t bps = ChangeEndian16(wfx.wBitsPerSample, Endian::Little);
				memcpy(newBuffer, &riff, 4);
				memcpy(newBuffer + 4, &chunkSize, 4);
				memcpy(newBuffer + 8, &riffType, 4);
				memcpy(newBuffer + 12, &fmt, 4);
				memcpy(newBuffer + 16, &subChunkSize, 4);
				memcpy(newBuffer + 20, &formatTag, 2);
				memcpy(newBuffer + 22, &nChannels, 2);
				memcpy(newBuffer + 24, &sampleRate, 4);
				memcpy(newBuffer + 28, &byteRate, 4);
				memcpy(newBuffer + 32, &blockAlign, 2);
				memcpy(newBuffer + 34, &bps, 2);
				memcpy(newBuffer + 36, &d, 4);
				memcpy(newBuffer + 40, &dataSizeL, 4);
				memcpy(newBuffer + headerSize, buffer.GetInnerBufferAddress(), dataSize);
				newFile->Write(newBuffer, fullBufferSize);
				free(newBuffer);
				return true;
			}
			return false;
		}
	}
}