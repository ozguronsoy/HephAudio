#include "WavFormat.h"
#include "HephException.h"
#include "AudioCodecManager.h"
#include "AudioProcessor.h"

using namespace HephAudio::Codecs;

constexpr uint32_t riffID = 0x52494646; // "RIFF"
constexpr uint32_t waveID = 0x057415645; // "WAVE"
constexpr uint32_t fmtID = 0x666D7420; // "fmt "
constexpr uint32_t dataID = 0x64617461; // "data"

namespace HephAudio
{
	namespace FileFormats
	{
		HephCommon::StringBuffer WavFormat::Extension() const
		{
			return ".wav .wave";
		}
		size_t WavFormat::FileFrameCount(const HephCommon::File* pAudioFile, const AudioFormatInfo& audioFormatInfo) const
		{
			uint32_t data32 = 0;

			pAudioFile->SetOffset(12);
			pAudioFile->Read(&data32, 4, Endian::Big);
			while (data32 != dataID)
			{
				pAudioFile->Read(&data32, 4, Endian::Little);
				if (pAudioFile->GetOffset() + data32 >= pAudioFile->FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WavFormat::FileFrameCount", "Failed to read the file. File might be corrupted."));
				}

				pAudioFile->IncreaseOffset(data32);
				pAudioFile->Read(&data32, 4, Endian::Big);
			}
			pAudioFile->Read(&data32, 4, Endian::Little);

			return data32 / audioFormatInfo.FrameSize();
		}
		AudioFormatInfo WavFormat::ReadAudioFormatInfo(const HephCommon::File* pAudioFile) const
		{
			AudioFormatInfo formatInfo;
			uint32_t data32, chunkSize;

			pAudioFile->SetOffset(0);

			pAudioFile->Read(&data32, 4, Endian::Big);
			if (data32 != riffID)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}

			pAudioFile->IncreaseOffset(4);
			pAudioFile->Read(&data32, 4, Endian::Big);
			if (data32 != waveID)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}

			pAudioFile->Read(&data32, 4, Endian::Big);
			while (data32 != fmtID)
			{
				pAudioFile->Read(&chunkSize, 4, Endian::Little);
				if (pAudioFile->GetOffset() + chunkSize >= pAudioFile->FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
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
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
				}

				pAudioFile->IncreaseOffset(chunkSize);
				pAudioFile->Read(&data32, 4, Endian::Big);
			}

			return formatInfo;
		}
		AudioBuffer WavFormat::ReadFile(const HephCommon::File* pAudioFile) const
		{
			const AudioFormatInfo wavFormatInfo = this->ReadAudioFormatInfo(pAudioFile);

			IAudioCodec* pAudioCodec = AudioCodecManager::FindCodec(wavFormatInfo.formatTag);
			if (pAudioCodec == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WavFormat::ReadFile", "Unsupported audio codec."));
			}

			uint32_t wavAudioDataSize;
			pAudioFile->Read(&wavAudioDataSize, 4, Endian::Little);

			const uint8_t bytesPerSample = wavFormatInfo.bitsPerSample / 8;

			EncodedBufferInfo encodedBufferInfo;
			encodedBufferInfo.pBuffer = malloc(wavAudioDataSize);
			if (encodedBufferInfo.pBuffer == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "WavFormat::ReadFile", "Insufficient memory."));
			}
			pAudioFile->ReadToBuffer(encodedBufferInfo.pBuffer, bytesPerSample, wavAudioDataSize / bytesPerSample);

			encodedBufferInfo.size_byte = wavAudioDataSize;
			encodedBufferInfo.size_frame = wavAudioDataSize / wavFormatInfo.FrameSize();
			encodedBufferInfo.formatInfo = wavFormatInfo;
			encodedBufferInfo.endian = Endian::Little;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(encodedBufferInfo.pBuffer);

			return hephaudioBuffer;
		}
		AudioBuffer WavFormat::ReadFile(const HephCommon::File* pAudioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) const
		{
			const uint8_t bytesPerSample = audioFormatInfo.bitsPerSample / 8;
			const size_t wavAudioDataSize = frameCount * audioFormatInfo.FrameSize();
			uint32_t data32 = 0;

			if (finishedPlaying != nullptr)
			{
				(*finishedPlaying) = false;
			}

			const uint32_t totalFrameCount = this->FileFrameCount(pAudioFile, audioFormatInfo);

			if (frameIndex > totalFrameCount)
			{
				RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WavFormat::ReadFile", "Frame index out of bounds."));
				return AudioBuffer();
			}
			pAudioFile->IncreaseOffset(frameIndex * audioFormatInfo.FrameSize());

			if (frameIndex + frameCount >= totalFrameCount)
			{
				frameCount = totalFrameCount - frameIndex;
				if (finishedPlaying != nullptr)
				{
					(*finishedPlaying) = true;
				}
			}

			EncodedBufferInfo encodedBufferInfo;
			encodedBufferInfo.pBuffer = malloc(wavAudioDataSize);
			if (encodedBufferInfo.pBuffer == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "WavFormat::ReadFile", "Insufficient memory."));
			}
			pAudioFile->ReadToBuffer(encodedBufferInfo.pBuffer, bytesPerSample, wavAudioDataSize / bytesPerSample);

			encodedBufferInfo.size_byte = wavAudioDataSize;
			encodedBufferInfo.size_frame = frameCount;
			encodedBufferInfo.formatInfo = audioFormatInfo;
			encodedBufferInfo.endian = Endian::Little;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(encodedBufferInfo.pBuffer);

			return hephaudioBuffer;
		}
		bool WavFormat::SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const
		{
			try
			{
				const HephCommon::File audioFile(filePath, overwrite ? HephCommon::FileOpenMode::WriteOverride : HephCommon::FileOpenMode::Write);
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

				if (HephCommon::File::GetSystemEndian() == Endian::Big)
				{
					AudioProcessor::ChangeEndian(buffer);
				}

				audioFile.WriteToBuffer(buffer.Begin(), bufferFormatInfo.bitsPerSample / 8, buffer.Size() / (bufferFormatInfo.bitsPerSample / 8));
			}
			catch (HephCommon::HephException)
			{
				return false;
			}

			return true;
		}
	}
}