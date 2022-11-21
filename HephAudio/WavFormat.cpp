#include "WavFormat.h"
#include "AudioException.h"
#include "AudioProcessor.h"

namespace HephAudio
{
	namespace Formats
	{
		std::wstring WavFormat::Extension() const noexcept
		{
			return L".wav .wave";
		}
		AudioFormatInfo WavFormat::ReadFormatInfo(const AudioFile& file, size_t& audioDataSize) const
		{
			void* audioFileBuffer = file.GetInnerBufferAddress();
			uint32_t subChunkSize, nextChunk = 0;
			AudioFormatInfo wfx = AudioFormatInfo();
			if (Read<uint32_t>(audioFileBuffer, 0, GetSystemEndian()) == *(uint32_t*)"RIFF")
			{
				if (Read<uint32_t>(audioFileBuffer, 8, GetSystemEndian()) == *(uint32_t*)"WAVE")
				{
					wfx.formatTag = Read<uint16_t>(audioFileBuffer, 20, Endian::Little);
					if (wfx.formatTag == WAVE_FORMAT_PCM || wfx.formatTag == WAVE_FORMAT_EXTENSIBLE || wfx.formatTag == WAVE_FORMAT_ALAW || wfx.formatTag == WAVE_FORMAT_MULAW)
					{
						wfx.channelCount = Read<uint16_t>(audioFileBuffer, 22, Endian::Little);
						wfx.sampleRate = Read<uint32_t>(audioFileBuffer, 24, Endian::Little);
						wfx.bitsPerSample = Read<uint16_t>(audioFileBuffer, 34, Endian::Little);
						wfx.headerSize = nextChunk + 8; // use cbSize as headerSize.
						if (wfx.formatTag == WAVE_FORMAT_EXTENSIBLE) // WAVE_FORMAT_EXTENSIBLE
						{
							wfx.formatTag = Read<uint16_t>(audioFileBuffer, 44, Endian::Little);
						}
						subChunkSize = Read<uint32_t>(audioFileBuffer, 16, Endian::Little);
						nextChunk = subChunkSize + 20;
						while (Read<uint32_t>(audioFileBuffer, nextChunk, GetSystemEndian()) != *(uint32_t*)"data")
						{
							const uint32_t chunkSize = Read<uint32_t>(audioFileBuffer, nextChunk + 4, Endian::Little);
							if (nextChunk + chunkSize + 8 >= file.Size())
							{
								throw AudioException(E_FAIL, L"WavFormat", L"Failed to read the wav file. File might be corrupted.");
							}
							nextChunk += chunkSize + 8;
						}
						audioDataSize = Read<uint32_t>(audioFileBuffer, nextChunk + 4, Endian::Little);
					}
					else
					{
						throw AudioException(E_FAIL, L"WavFormat", L"Compression not supported.");
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
		AudioBuffer WavFormat::ReadFile(const AudioFile& file) const
		{
			size_t audioDataSize;
			AudioFormatInfo waveFormat = ReadFormatInfo(file, audioDataSize);
			size_t frameCount = audioDataSize / waveFormat.FrameSize();
			AudioBuffer resultBuffer = AudioBuffer(frameCount, waveFormat);
			memcpy(resultBuffer.Begin(), (uint8_t*)file.GetInnerBufferAddress() + waveFormat.headerSize, audioDataSize);
			if (GetSystemEndian() == Endian::Big && waveFormat.bitsPerSample != 8) // switch bytes.
			{
				uint8_t* innerBuffer = (uint8_t*)resultBuffer.Begin();
				const uint32_t sampleSize = waveFormat.bitsPerSample / 8;
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
			AudioProcessor::ConvertPcmToInnerFormat(resultBuffer);
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
				AudioFormatInfo wfx = buffer.GetFormat();
				const uint16_t formatTag = ChangeEndian16(wfx.formatTag, Endian::Little);
				const uint16_t channelCount = ChangeEndian16(wfx.channelCount, Endian::Little);
				const uint32_t sampleRate = ChangeEndian32(wfx.sampleRate, Endian::Little);
				const uint32_t byteRate = ChangeEndian32(wfx.ByteRate(), Endian::Little);
				const uint16_t blockAlign = ChangeEndian16(wfx.FrameSize(), Endian::Little);
				const uint16_t bps = ChangeEndian16(wfx.bitsPerSample, Endian::Little);
				memcpy(newBuffer, &riff, 4);
				memcpy(newBuffer + 4, &chunkSize, 4);
				memcpy(newBuffer + 8, &riffType, 4);
				memcpy(newBuffer + 12, &fmt, 4);
				memcpy(newBuffer + 16, &subChunkSize, 4);
				memcpy(newBuffer + 20, &formatTag, 2);
				memcpy(newBuffer + 22, &channelCount, 2);
				memcpy(newBuffer + 24, &sampleRate, 4);
				memcpy(newBuffer + 28, &byteRate, 4);
				memcpy(newBuffer + 32, &blockAlign, 2);
				memcpy(newBuffer + 34, &bps, 2);
				memcpy(newBuffer + 36, &d, 4);
				memcpy(newBuffer + 40, &dataSizeL, 4);
				memcpy(newBuffer + headerSize, buffer.Begin(), dataSize);
				newFile->Write(newBuffer, fullBufferSize);
				free(newBuffer);
				return true;
			}
			return false;
		}
	}
}