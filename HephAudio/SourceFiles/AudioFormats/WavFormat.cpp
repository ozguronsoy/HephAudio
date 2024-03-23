#include "AudioFormats/WavFormat.h"
#include "AudioProcessor.h"
#include "AudioCodecs/AudioCodecManager.h"
#include "../HephCommon/HeaderFiles/HephException.h"

using namespace HephCommon;
using namespace HephAudio::Codecs;

static constexpr uint32_t riffID = 0x52494646; // "RIFF"
static constexpr uint32_t waveID = 0x057415645; // "WAVE"
static constexpr uint32_t fmtID = 0x666D7420; // "fmt "
static constexpr uint32_t dataID = 0x64617461; // "data"


namespace HephAudio
{
	namespace FileFormats
	{
		StringBuffer WavFormat::Extensions()
		{
			return ".wav .wave";
		}
		bool WavFormat::VerifySignature(const File& audioFile)
		{
			uint32_t data32 = 0;

			audioFile.SetOffset(0);
			audioFile.Read(&data32, 4, Endian::Big);
			if (data32 == riffID)
			{
				audioFile.SetOffset(8);
				audioFile.Read(&data32, 4, Endian::Big);
				return data32 == waveID;
			}

			return false;
		}
		size_t WavFormat::FileFrameCount(const File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
#else
			uint32_t data32 = 0;

			audioFile.SetOffset(12);
			audioFile.Read(&data32, 4, Endian::Big);
			while (data32 != dataID)
			{
				audioFile.Read(&data32, 4, Endian::Little);
				if (audioFile.GetOffset() + data32 >= audioFile.FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "WavFormat::FileFrameCount", "Failed to read the file. File might be corrupted."));
				}

				audioFile.IncreaseOffset(data32);
				audioFile.Read(&data32, 4, Endian::Big);
			}
			audioFile.Read(&data32, 4, Endian::Little);

			return data32 / audioFormatInfo.FrameSize();
#endif
		}
		AudioFormatInfo WavFormat::ReadAudioFormatInfo(const File& audioFile)
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
#else
			AudioFormatInfo formatInfo;
			uint32_t data32 = 0, chunkSize = 0;

			audioFile.SetOffset(0);

			audioFile.Read(&data32, 4, Endian::Big);
			if (data32 != riffID)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}

			audioFile.IncreaseOffset(4);
			audioFile.Read(&data32, 4, Endian::Big);
			if (data32 != waveID)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}

			audioFile.Read(&data32, 4, Endian::Big);
			while (data32 != fmtID)
			{
				audioFile.Read(&chunkSize, 4, Endian::Little);
				if (audioFile.GetOffset() + chunkSize >= audioFile.FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
				}

				audioFile.IncreaseOffset(chunkSize);
				audioFile.Read(&data32, 4, Endian::Big);
			}
			audioFile.Read(&chunkSize, 4, Endian::Little);
			const uint64_t fmtChunkEnd = audioFile.GetOffset() + chunkSize;

			audioFile.Read(&formatInfo.formatTag, 2, Endian::Little);
			audioFile.Read(&formatInfo.channelCount, 2, Endian::Little);
			audioFile.Read(&formatInfo.sampleRate, 4, Endian::Little);

			uint32_t byteRate;
			audioFile.Read(&byteRate, 4, Endian::Little);
			formatInfo.bitRate = byteRate * 8;

			audioFile.IncreaseOffset(2);
			audioFile.Read(&formatInfo.bitsPerSample, 2, Endian::Little);

			if (formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_EXTENSIBLE)
			{
				uint16_t extensionSize;
				audioFile.Read(&extensionSize, 2, Endian::Little);
				audioFile.IncreaseOffset(extensionSize - 16);
				audioFile.Read(&formatInfo.formatTag, 2, Endian::Little);
			}

			audioFile.SetOffset(fmtChunkEnd);
			audioFile.Read(&data32, 4, Endian::Big);
			while (data32 != dataID)
			{
				audioFile.Read(&chunkSize, 4, Endian::Little);
				if (audioFile.GetOffset() + chunkSize >= audioFile.FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "WavFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
				}

				audioFile.IncreaseOffset(chunkSize);
				audioFile.Read(&data32, 4, Endian::Big);
			}

			return formatInfo;
#endif
		}
		AudioBuffer WavFormat::ReadFile(const File& audioFile)
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
#else
			const AudioFormatInfo wavFormatInfo = this->ReadAudioFormatInfo(audioFile);

			IAudioCodec* pAudioCodec = AudioCodecManager::FindCodec(wavFormatInfo.formatTag);
			if (pAudioCodec == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "WavFormat::ReadFile", "Unsupported audio codec."));
			}

			uint32_t wavAudioDataSize = 0;
			audioFile.Read(&wavAudioDataSize, 4, Endian::Little);

			const uint8_t bytesPerSample = wavFormatInfo.bitsPerSample / 8;

			EncodedBufferInfo encodedBufferInfo;
			encodedBufferInfo.pBuffer = malloc(wavAudioDataSize);
			if (encodedBufferInfo.pBuffer == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "WavFormat::ReadFile", "Insufficient memory."));
			}
			audioFile.ReadToBuffer(encodedBufferInfo.pBuffer, bytesPerSample, wavAudioDataSize / bytesPerSample);

			encodedBufferInfo.size_byte = wavAudioDataSize;
			encodedBufferInfo.size_frame = wavAudioDataSize / wavFormatInfo.FrameSize();
			encodedBufferInfo.formatInfo = wavFormatInfo;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(encodedBufferInfo.pBuffer);

			return hephaudioBuffer;
#endif
	}
		AudioBuffer WavFormat::ReadFile(const File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying)
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			const AudioBuffer decodedBuffer = this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
			if (finishedPlaying != nullptr)
			{
				*finishedPlaying = (frameIndex + frameCount) >= this->ffmpegAudioDecoder.GetFrameCount();
			}
			return decodedBuffer;
#else
			const uint8_t bytesPerSample = audioFormatInfo.bitsPerSample / 8;
			const size_t wavAudioDataSize = frameCount * audioFormatInfo.FrameSize();
			uint32_t data32 = 0;

			if (finishedPlaying != nullptr)
			{
				(*finishedPlaying) = false;
			}

			const uint32_t totalFrameCount = this->FileFrameCount(audioFile, audioFormatInfo);

			if (frameIndex > totalFrameCount)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "WavFormat::ReadFile", "Frame index out of bounds."));
				return AudioBuffer();
			}
			audioFile.IncreaseOffset(frameIndex * audioFormatInfo.FrameSize());

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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "WavFormat::ReadFile", "Insufficient memory."));
			}
			audioFile.ReadToBuffer(encodedBufferInfo.pBuffer, bytesPerSample, wavAudioDataSize / bytesPerSample);

			encodedBufferInfo.size_byte = wavAudioDataSize;
			encodedBufferInfo.size_frame = frameCount;
			encodedBufferInfo.formatInfo = audioFormatInfo;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(encodedBufferInfo.pBuffer);

			return hephaudioBuffer;
#endif
}
		bool WavFormat::SaveToFile(const StringBuffer& filePath, AudioBuffer& buffer, bool overwrite)
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
			try
			{
				FFmpegAudioEncoder ffmpegAudioEncoder(filePath, buffer.FormatInfo(), overwrite);
				ffmpegAudioEncoder.Encode(buffer);
			}
			catch (HephException)
			{
				return false;
			}

			return true;
#else
			try
			{
				const File audioFile(filePath, overwrite ? FileOpenMode::Overwrite : FileOpenMode::Write);
				const AudioFormatInfo& bufferFormatInfo = buffer.FormatInfo();
				uint16_t data16 = 0;
				uint32_t data32 = 0;

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

				if (bufferFormatInfo.endian == Endian::Big)
				{
					AudioProcessor::ChangeEndian(buffer);
				}

				audioFile.WriteFromBuffer(buffer.Begin(), bufferFormatInfo.bitsPerSample / 8, buffer.Size() / (bufferFormatInfo.bitsPerSample / 8));
			}
			catch (HephException)
			{
				return false;
			}

			return true;
#endif
		}
	}
}