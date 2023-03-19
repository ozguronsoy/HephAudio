#include "WavFormat.h"
#include "AudioException.h"
#include "AudioCodecManager.h"

using namespace HephAudio::Codecs;

constexpr uint32_t riffID = 0x52494646; // "RIFF"
constexpr uint32_t waveID = 0x057415645; // "WAVE"
constexpr uint32_t fmtID = 0x666d7420; // "fmt "
constexpr uint32_t dataID = 0x64617461; // "data"

namespace HephAudio
{
	namespace Formats
	{
		StringBuffer WavFormat::Extension() const
		{
			return ".wav .wave";
		}
		AudioFormatInfo WavFormat::ReadAudioFormatInfo(const AudioFile* pAudioFile) const
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
			const uint64_t fmtChunkEnd = pAudioFile->GetOffset() + chunkSize;

			pAudioFile->Read(&formatInfo.formatTag, 2, Endian::Little);
			pAudioFile->Read(&formatInfo.channelCount, 2, Endian::Little);
			pAudioFile->Read(&formatInfo.sampleRate, 4, Endian::Little);
			pAudioFile->IncreaseOffset(6);
			pAudioFile->Read(&formatInfo.bitsPerSample, 2, Endian::Little);

			if (formatInfo.formatTag == WAVE_FORMAT_EXTENSIBLE)
			{
				uint16_t extensionSize;
				pAudioFile->Read(&extensionSize, 2, Endian::Little);
				pAudioFile->IncreaseOffset(extensionSize - 16);
				pAudioFile->Read(&formatInfo.formatTag, 2, Endian::Little);
			}

			pAudioFile->SetOffset(fmtChunkEnd);
			pAudioFile->Read(&data32, 4, Endian::Big);
			while (data32 != dataID)
			{
				pAudioFile->Read(&chunkSize, 4, Endian::Little);
				if (pAudioFile->GetOffset() + chunkSize >= pAudioFile->FileSize())
				{
					throw AudioException(E_FAIL, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted.");
				}

				pAudioFile->IncreaseOffset(chunkSize);
				pAudioFile->Read(&data32, 4, Endian::Big);
			}

			return formatInfo;
		}
		AudioBuffer WavFormat::ReadFile(const AudioFile* pAudioFile) const
		{
			const AudioFormatInfo wavFormatInfo = ReadAudioFormatInfo(pAudioFile);

			IAudioCodec* pAudioCodec = AudioCodecManager::FindCodec(wavFormatInfo.formatTag);
			if (pAudioCodec == nullptr)
			{
				throw AudioException(E_FAIL, "WavFormat::ReadFile", "Unsupported audio codec.");
			}

			uint32_t wavAudioDataSize;
			pAudioFile->Read(&wavAudioDataSize, 4, Endian::Little);

			const uint8_t bytesPerSample = wavFormatInfo.bitsPerSample / 8;
			void* pPcmBuffer = malloc(wavAudioDataSize);
			if (pPcmBuffer == nullptr)
			{
				throw AudioException(E_OUTOFMEMORY, "WavFormat::ReadFile", "Insufficient memory.");
			}
			pAudioFile->ReadToBuffer(pPcmBuffer, bytesPerSample, wavAudioDataSize / bytesPerSample);

			EncodedBufferInfo encodedBufferInfo;
			encodedBufferInfo.pBuffer = pPcmBuffer;
			encodedBufferInfo.size_byte = wavAudioDataSize;
			encodedBufferInfo.size_frame = wavAudioDataSize / wavFormatInfo.FrameSize();
			encodedBufferInfo.formatInfo = wavFormatInfo;
			encodedBufferInfo.endian = Endian::Little;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(pPcmBuffer);

			return hephaudioBuffer;
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