#include "AiffFormat.h"
#include "AudioException.h"
#include "AudioProcessor.h"

namespace HephAudio
{
	namespace Formats
	{
		std::wstring AiffFormat::Extension() const noexcept
		{
			return L".aiff .aifc .aif";
		}
		AudioFormatInfo AiffFormat::ReadFormatInfo(const AudioFile& inFile, uint32_t& outFrameCount, Endian& outEndian) const
		{
			AudioFormatInfo format = AudioFormatInfo();
			void* audioFileBuffer = inFile.GetInnerBufferAddress();
			outEndian = Endian::Big;
			if (Read<uint32_t>(audioFileBuffer, 0, GetSystemEndian()) == *(uint32_t*)"FORM")
			{
				const uint32_t formType = Read<uint32_t>(audioFileBuffer, 8, GetSystemEndian());
				if (formType == *(uint32_t*)"AIFF" || formType == *(uint32_t*)"AIFC")
				{
					uint32_t cursor = 12;
					if (Read<uint32_t>(audioFileBuffer, cursor, GetSystemEndian()) == *(uint32_t*)"FVER") // ignore version.
					{
						cursor += 12;
					}
					if (Read<uint32_t>(audioFileBuffer, cursor, GetSystemEndian()) == *(uint32_t*)"COMM")
					{
						uint32_t commSize = Read<uint32_t>(audioFileBuffer, cursor + 4, Endian::Big);
						if (commSize % 2 == 1)
						{
							commSize += 1; // Chunk size must be an even number, if not a padding byte is added. The padding byte is not included in chunk size, thus add one.
						}
						uint32_t nextChunk = cursor + commSize + 8;
						cursor += 8;
						format.channelCount = Read<uint16_t>(audioFileBuffer, cursor, Endian::Big);
						cursor += 2;
						outFrameCount = Read<uint32_t>(audioFileBuffer, cursor, Endian::Big);
						cursor += 4;
						format.bitsPerSample = Read<uint16_t>(audioFileBuffer, cursor, Endian::Big);
						cursor += 2;
						SampleRateFrom64(Read<uint64_t>(audioFileBuffer, cursor, Endian::Big), &format);
						cursor += 10;
						if (formType == *(uint32_t*)"AIFC")
						{
							const uint32_t compressionType = Read<uint32_t>(audioFileBuffer, cursor, GetSystemEndian());
							if (compressionType != *(uint32_t*)"NONE" && compressionType != *(uint32_t*)"sowt")
							{
								throw AudioException(E_FAIL, L"AiffFormat", L"Compressed AIFF files are not supported.");
							}
							if (compressionType == *(uint32_t*)"sowt")
							{
								outEndian = Endian::Little;
							}
						}
						format.formatTag = 1;
						format.headerSize = nextChunk;
					}
					else
					{
						throw AudioException(E_FAIL, L"AiffFormat", L"Failed to read the aiff file. File might be corrupted.");
					}
				}
				else
				{
					throw AudioException(E_FAIL, L"AiffFormat", L"Failed to read the aiff file. File might be corrupted.");
				}
			}
			else
			{
				throw AudioException(E_FAIL, L"AiffFormat", L"Failed to read the aiff file. File might be corrupted.");
			}
			return format;
		}
		AudioBuffer AiffFormat::ReadFile(const AudioFile& file) const
		{
			uint32_t frameCount = 0;
			Endian audioDataEndian;
			AudioFormatInfo format = ReadFormatInfo(file, frameCount, audioDataEndian);
			void* audioFileBuffer = file.GetInnerBufferAddress();
			uint32_t cursor = format.headerSize;
			while (true) // Find the sound data chunk.
			{
				if (Read<uint32_t>(audioFileBuffer, cursor, GetSystemEndian()) != *(uint32_t*)"SSND")
				{
					uint32_t chunkSize = Read<uint32_t>(audioFileBuffer, cursor + 4, Endian::Big) + 8;
					if (chunkSize % 2 == 1)
					{
						chunkSize += 1; // Chunk size must be an even number, if not a padding byte is added. The padding byte is not included in chunk size, thus add one.
					}
					if (cursor + chunkSize >= file.Size())
					{
						throw AudioException(E_FAIL, L"AiffFormat::ReadFile", L"Failed to read the aiff file. File might be corrupted.");
					}
					cursor += chunkSize;
				}
				else
				{
					break;
				}
			}
			cursor += 16;
			AudioBuffer buffer(frameCount, format);
			memcpy(buffer.Begin(), (uint8_t*)audioFileBuffer + cursor, buffer.Size());
			if (audioDataEndian != GetSystemEndian() && format.bitsPerSample != 8) // switch bytes.
			{
				uint8_t* innerBuffer = (uint8_t*)buffer.Begin();
				const uint32_t sampleSize = format.bitsPerSample / 8;
				for (size_t i = 0; i < buffer.Size(); i += sampleSize)
				{
					switch (sampleSize)
					{
					case 2:
					{
						uint16_t sample = Read<uint16_t>(innerBuffer, i, audioDataEndian);
						memcpy(innerBuffer + i, &sample, 2);
					}
					break;
					case 3:
					{
						uint32_t sample = Read<uint32_t>(innerBuffer, i, audioDataEndian) >> 8;
						memcpy(innerBuffer + i, &sample, 3);
					}
					break;
					case 4:
					{
						uint32_t sample = Read<uint32_t>(innerBuffer, i, audioDataEndian);
						memcpy(innerBuffer + i, &sample, 4);
					}
					break;
					default:
						throw AudioException(E_FAIL, L"AiffFormat::ReadFile", L"Invalid bps.");
					}
				}
			}
			AudioProcessor::ConvertPcmToInnerFormat(buffer);
			return buffer;
		}
		bool AiffFormat::SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const
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
			AudioFormatInfo bufferFormat = buffer.FormatInfo();
			const uint64_t srBits = ChangeEndian64(SampleRateTo64(&bufferFormat), Endian::Big);
			const size_t headerSize = 68;
			const size_t padding = buffer.Size() % 2 == 1 ? 1 : 0;
			const size_t fullFileSize = buffer.Size() + headerSize + padding;
			const uint32_t form = *(uint32_t*)"FORM";
			const uint32_t aiff = *(uint32_t*)"AIFC";
			const uint32_t fver = *(uint32_t*)"FVER";
			const uint32_t comm = *(uint32_t*)"COMM";
			const uint32_t ssnd = *(uint32_t*)"SSND";
			const uint32_t compressionType = GetSystemEndian() == Endian::Big ? *(uint32_t*)"NONE" : *(uint32_t*)"sowt";
			const std::string compressionName = "not compressed";
			uint8_t* newBuffer = (uint8_t*)malloc(fullFileSize);
			if (newBuffer != nullptr)
			{
				const uint16_t channelCount = ChangeEndian16(bufferFormat.channelCount, Endian::Big);
				const uint16_t bps = ChangeEndian16(bufferFormat.bitsPerSample, Endian::Big);
				const uint32_t four32 = ChangeEndian32(4, Endian::Big);
				const uint32_t timeStamp = ChangeEndian32(0xA2805140, Endian::Big);
				const uint32_t commSize = ChangeEndian32(headerSize - 32, Endian::Big);
				const uint32_t frameCount = ChangeEndian32(buffer.FrameCount(), Endian::Big);
				const uint32_t sndByteCount = ChangeEndian32(buffer.FrameCount() * bufferFormat.FrameSize() + 8, Endian::Big);
				memcpy(newBuffer, &form, 4);
				memcpy(newBuffer + 4, &four32, 4);
				memcpy(newBuffer + 8, &aiff, 4);
				memcpy(newBuffer + 12, &fver, 4);
				memcpy(newBuffer + 16, &four32, 4);
				memcpy(newBuffer + 20, &timeStamp, 4);
				memcpy(newBuffer + 24, &comm, 4);
				memcpy(newBuffer + 28, &commSize, 4);
				memcpy(newBuffer + 32, &channelCount, 2);
				memcpy(newBuffer + 34, &frameCount, 4);
				memcpy(newBuffer + 38, &bps, 2);
				memcpy(newBuffer + 40, &srBits, 8);
				memset(newBuffer + 48, 0, 2);
				memcpy(newBuffer + 50, &compressionType, 4);
				memcpy(newBuffer + 54, &compressionName.at(0), 14);
				memcpy(newBuffer + 68, &ssnd, 4);
				memcpy(newBuffer + 72, &sndByteCount, 4);
				memset(newBuffer + 76, 0, 8);
				memcpy(newBuffer + 84, buffer.Begin(), buffer.Size());
				if (padding == 1)
				{
					memset(newBuffer + 84 + buffer.Size(), 0, 1);
				}
				newFile->Write(newBuffer, fullFileSize);
				free(newBuffer);
				return true;
			}
			return false;
		}
		void AiffFormat::SampleRateFrom64(uint64_t srBits, AudioFormatInfo* format) const
		{
			if (format == nullptr)
			{
				throw AudioException(E_INVALIDARG, L"AiffFormat::SampleRateFrom64", L"format was nullptr.");
			}
			if (srBits == 0x400FAC4400000000)
			{
				format->sampleRate = 88200;
			}
			else if (srBits == 0x400EAC4400000000)
			{
				format->sampleRate = 44100;
			}
			else if (srBits == 0x400DAC4400000000)
			{
				format->sampleRate = 22050;
			}
			else if (srBits == 0x400CAC4400000000)
			{
				format->sampleRate = 11025;
			}
			else if (srBits == 0x400FBB8000000000)
			{
				format->sampleRate = 96000;
			}
			else if (srBits == 0x400EBB8000000000)
			{
				format->sampleRate = 48000;
			}
			else if (srBits == 0x400DBB8000000000)
			{
				format->sampleRate = 24000;
			}
			else if (srBits == 0x400CBB8000000000)
			{
				format->sampleRate = 12000;
			}
			else if (srBits == 0x400BBB8000000000)
			{
				format->sampleRate = 6000;
			}
			else if (srBits == 0x400FFA0000000000)
			{
				format->sampleRate = 128000;
			}
			else if (srBits == 0x400EFA0000000000)
			{
				format->sampleRate = 64000;
			}
			else if (srBits == 0x400DFA0000000000)
			{
				format->sampleRate = 32000;
			}
			else if (srBits == 0x400CFA0000000000)
			{
				format->sampleRate = 16000;
			}
			else if (srBits == 0x400BFA0000000000)
			{
				format->sampleRate = 8000;
			}
			else if (srBits == 0x400AFA0000000000)
			{
				format->sampleRate = 4000;
			}
			else if (srBits == 0x4009FA0000000000)
			{
				format->sampleRate = 2000;
			}
			else if (srBits == 0x4008FA0000000000)
			{
				format->sampleRate = 1000;
			}
			else
			{
				throw AudioException(E_FAIL, L"AiffFormat", L"Unknown sample rate.");
			}
		}
		uint64_t AiffFormat::SampleRateTo64(const AudioFormatInfo* const& format) const
		{
			if (format == nullptr)
			{
				throw AudioException(E_INVALIDARG, L"AiffFormat::SampleRateTo64", L"format was nullptr.");
			}
			if (format->sampleRate == 88200)
			{
				return 0x400FAC4400000000;
			}
			else if (format->sampleRate == 44100)
			{
				return 0x400EAC4400000000;
			}
			else if (format->sampleRate == 22050)
			{
				return 0x400DAC4400000000;
			}
			else if (format->sampleRate == 11025)
			{
				return 0x400CAC4400000000;
			}
			else if (format->sampleRate == 96000)
			{
				return 0x400FBB8000000000;
			}
			else if (format->sampleRate == 48000)
			{
				return 0x400EBB8000000000;
			}
			else if (format->sampleRate == 24000)
			{
				return 0x400DBB8000000000;
			}
			else if (format->sampleRate == 12000)
			{
				return 0x400CBB8000000000;
			}
			else if (format->sampleRate == 6000)
			{
				return 0x400BBB8000000000;
			}
			else if (format->sampleRate == 128000)
			{
				return 0x400FFA0000000000;
			}
			else if (format->sampleRate == 64000)
			{
				return 0x400EFA0000000000;
			}
			else if (format->sampleRate == 32000)
			{
				return 0x400DFA0000000000;
			}
			else if (format->sampleRate == 16000)
			{
				return 0x400CFA0000000000;
			}
			else if (format->sampleRate == 8000)
			{
				return 0x400BFA0000000000;
			}
			else if (format->sampleRate == 4000)
			{
				return 0x400AFA0000000000;
			}
			else if (format->sampleRate == 2000)
			{
				return 0x4009FA0000000000;
			}
			else if (format->sampleRate == 1000)
			{
				return 0x4008FA0000000000;
			}
			throw AudioException(E_FAIL, L"AiffFormat", L"Unknown sample rate.");
		}
	}
}