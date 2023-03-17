#include "WavFormat.h"
#include "AudioException.h"
#include "AudioProcessor.h"

constexpr uint32_t riffID = 0x52494646; // "RIFF"
constexpr uint32_t waveID = 0x057415645; // "WAVE"
constexpr uint32_t fmtID = 0x666d7420; // "fmt "
constexpr uint32_t dataID = 0x64617461; // "data"

namespace HephAudio
{
	namespace Formats
	{
		StringBuffer WavFormat::Extension() const noexcept
		{
			return ".wav .wave";
		}
		AudioFormatInfo WavFormat::ReadAudioFormatInfo(const AudioFile* pAudioFile) const noexcept
		{
			AudioFormatInfo formatInfo;
			uint32_t data32, chunkSize;

			pAudioFile->Read(&data32, 4, Endian::Big);
			if (data32 != riffID)
			{
				throw AudioException(E_FAIL, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted.");
			}

			pAudioFile->IncreaseOffset(4);
			pAudioFile->Read(&data32, 4, Endian::Big);
			if (data32 != waveID)
			{
				throw AudioException(E_FAIL, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted.");
			}

			pAudioFile->Read(&data32, 4, Endian::Big);
			while (data32 != fmtID)
			{
				pAudioFile->Read(&chunkSize, 4, Endian::Little);
				if (pAudioFile->GetOffset() + chunkSize >= pAudioFile->FileSize())
				{
					throw AudioException(E_FAIL, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted.");
				}

				pAudioFile->IncreaseOffset(chunkSize);
				pAudioFile->Read(&data32, 4, Endian::Big);
			}

			pAudioFile->Read(&chunkSize, 4, Endian::Little);
			pAudioFile->Read(&formatInfo.formatTag, 2, Endian::Little);
			pAudioFile->Read(&formatInfo.channelCount, 2, Endian::Little);
			pAudioFile->Read(&formatInfo.sampleRate, 4, Endian::Little);
			pAudioFile->IncreaseOffset(6);
			pAudioFile->Read(&formatInfo.bitsPerSample, 2, Endian::Little);

			pAudioFile->Read(&data32, 4, Endian::Big);
			while (data32 != dataID)
			{
				uint32_t chunkSize;
				pAudioFile->Read(&chunkSize, 4, Endian::Little);
				pAudioFile->IncreaseOffset(chunkSize);
				pAudioFile->Read(&data32, 4, Endian::Big);
			}

			return formatInfo;
		}
		AudioBuffer WavFormat::ReadFile(const AudioFile* pAudioFile) const
		{
			const AudioFormatInfo wavFormatInfo = ReadAudioFormatInfo(pAudioFile);

			uint32_t wavAudioDataSize;
			pAudioFile->Read(&wavAudioDataSize, 4, Endian::Little);

			AudioBuffer buffer = AudioBuffer(wavAudioDataSize / wavFormatInfo.FrameSize(), AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, wavFormatInfo.channelCount, sizeof(hephaudio_float) * 8, wavFormatInfo.sampleRate));

			void* wavBuffer = malloc(wavAudioDataSize);
			if (wavBuffer == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "WavFormat::ReadFile", "Insufficient memory.");
			}
			pAudioFile->ReadToBuffer(wavBuffer, wavFormatInfo.bitsPerSample / 8, wavAudioDataSize / (wavFormatInfo.bitsPerSample / 8));

			if (AudioFile::GetSystemEndian() != Endian::Little)
			{
				switch (wavFormatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < wavFormatInfo.channelCount; j++)
						{
							buffer[i][j] = (hephaudio_float)((uint8_t*)wavBuffer)[i * wavFormatInfo.channelCount + j] / (hephaudio_float)UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < wavFormatInfo.channelCount; j++)
						{
							int16_t wavSample = ((int16_t*)wavBuffer)[i * wavFormatInfo.channelCount + j];
							AudioFile::ChangeEndian((uint8_t*)&wavSample, 2);

							buffer[i][j] = (hephaudio_float)wavSample / (hephaudio_float)(INT16_MAX + 1);
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < wavFormatInfo.channelCount; j++)
						{
							int24 wavSample = ((int24*)wavBuffer)[i * wavFormatInfo.channelCount + j];
							AudioFile::ChangeEndian((uint8_t*)&wavSample, 3);

							buffer[i][j] = (hephaudio_float)wavSample / (hephaudio_float)(INT24_MAX + 1);
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < wavFormatInfo.channelCount; j++)
						{
							int32_t wavSample = ((int32_t*)wavBuffer)[i * wavFormatInfo.channelCount + j];
							AudioFile::ChangeEndian((uint8_t*)&wavSample, 4);

							buffer[i][j] = (hephaudio_float)wavSample / (hephaudio_float)(INT32_MAX + 1ull);
						}
					}
				}
				break;
				default:
					throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
				}
			}
			else
			{
				switch (wavFormatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < wavFormatInfo.channelCount; j++)
						{
							buffer[i][j] = (hephaudio_float)((uint8_t*)wavBuffer)[i * wavFormatInfo.channelCount + j] / (hephaudio_float)UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < wavFormatInfo.channelCount; j++)
						{
							buffer[i][j] = (hephaudio_float)((int16_t*)wavBuffer)[i * wavFormatInfo.channelCount + j] / (hephaudio_float)(INT16_MAX + 1);
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < wavFormatInfo.channelCount; j++)
						{
							buffer[i][j] = (hephaudio_float)((int24*)wavBuffer)[i * wavFormatInfo.channelCount + j] / (hephaudio_float)(INT24_MAX + 1);
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < buffer.FrameCount(); i++)
					{
						for (size_t j = 0; j < wavFormatInfo.channelCount; j++)
						{
							buffer[i][j] = (hephaudio_float)((int32_t*)wavBuffer)[i * wavFormatInfo.channelCount + j] / (hephaudio_float)(INT32_MAX + 1ull);
						}
					}
				}
				break;
				default:
					throw AudioException(E_FAIL, "WavFormat::ReadFile", "Invalid sample size.");
				}
			}

			free(wavBuffer);

			return buffer;
		}
		bool WavFormat::SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const
		{
			try
			{
				const AudioFile audioFile = AudioFile(filePath, overwrite ? AudioFileOpenMode::WriteOverride : AudioFileOpenMode::Write);
				const AudioFormatInfo& bufferFormatInfo = buffer.FormatInfo();
				uint16_t data16;
				uint32_t data32;

				audioFile.Write(&riffID, 4, Endian::Big);
				data32 = 4;
				audioFile.Write(&data32, 4, Endian::Little);
				audioFile.Write(&waveID, 4, Endian::Big);

				audioFile.Write(&fmtID, 4, Endian::Big);
				data32 = 16;
				audioFile.Write(&data32, 4, Endian::Little);
				audioFile.Write(&bufferFormatInfo.formatTag, 2, Endian::Little);
				audioFile.Write(&bufferFormatInfo.channelCount, 2, Endian::Little);
				audioFile.Write(&bufferFormatInfo.sampleRate, 4, Endian::Little);
				data32 = bufferFormatInfo.ByteRate();
				audioFile.Write(&data32, 4, Endian::Little);
				data16 = bufferFormatInfo.FrameSize();
				audioFile.Write(&data16, 2, Endian::Little);
				audioFile.Write(&bufferFormatInfo.bitsPerSample, 2, Endian::Little);

				audioFile.Write(&dataID, 4, Endian::Big);
				data32 = buffer.Size();
				audioFile.Write(&data32, 4, Endian::Little);
				audioFile.WriteToBuffer(buffer.Begin(), bufferFormatInfo.bitsPerSample / 8, buffer.Size() / (bufferFormatInfo.bitsPerSample / 8));
			}
			catch (AudioException)
			{
				return false;
			}

			return true;
		}
	}
}