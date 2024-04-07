#include "AudioFormats/AiffFormat.h"
#include "AudioProcessor.h"
#include "AudioCodecs/AudioCodecManager.h"
#include "HephException.h"

#define AIFC_SOWT (HEPHAUDIO_FORMAT_TAG_PCM << 8)

using namespace HephCommon;
using namespace HephAudio::Codecs;

namespace HephAudio
{
	namespace FileFormats
	{
		static constexpr uint32_t formID = 0x464F524D; // "FORM"
		static constexpr uint32_t fverID = 0x46564552; // "FVER"
		static constexpr uint32_t aiffID = 0x41494646; // "AIFF"
		static constexpr uint32_t aifcID = 0x41494643; // "AIFC"
		static constexpr uint32_t commID = 0x434F4D4D; // "COMM"
		static constexpr uint32_t ssndID = 0x53534E44; // "SSND"
		static constexpr uint32_t aifc_v1 = 0xA2805140;

		std::string AiffFormat::Extensions()
		{
			return "aif aiff aifc";
		}
		bool AiffFormat::VerifySignature(const File& audioFile)
		{
			uint32_t data32 = 0;

			audioFile.SetOffset(0);
			audioFile.Read(&data32, 4, Endian::Big);
			if (data32 == formID)
			{
				audioFile.IncreaseOffset(4);
				audioFile.Read(&data32, 4, Endian::Big);
				return data32 == aiffID || data32 == aifcID;
			}

			return false;
		}
		size_t AiffFormat::FileFrameCount(const File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
#else
			uint32_t data32 = 0;

			audioFile.SetOffset(0);
			audioFile.Read(&data32, 4, Endian::Big);
			while (data32 != ssndID)
			{
				audioFile.Read(&data32, 4, Endian::Big);
				if (data32 % 2 == 1)
				{
					data32 += 1;
				}
				if (audioFile.GetOffset() + data32 >= audioFile.FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat::FileFrameCount", "Failed to read the file. File might be corrupted."));
				}

				audioFile.IncreaseOffset(data32);
				audioFile.Read(&data32, 4, Endian::Big);
			}
			audioFile.Read(&data32, 4, Endian::Big);

			return data32 / audioFormatInfo.FrameSize();
#endif
		}
		AudioFormatInfo AiffFormat::ReadAudioFormatInfo(const File& audioFile)
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
#else
			AudioFormatInfo formatInfo;
			uint32_t data32 = 0, formType = 0, chunkSize = 0;
			uint64_t srBits = 0;

			audioFile.SetOffset(0);

			audioFile.Read(&data32, 4, Endian::Big);
			if (data32 != formID)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}
			audioFile.IncreaseOffset(4);
			audioFile.Read(&formType, 4, Endian::Big);
			if (formType != aiffID && formType != aifcID)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
			}

			audioFile.Read(&data32, 4, Endian::Big);
			while (data32 != commID)
			{
				audioFile.Read(&chunkSize, 4, Endian::Big);
				if (chunkSize % 2 == 1)
				{
					chunkSize += 1;
				}
				if (audioFile.GetOffset() + chunkSize >= audioFile.FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
				}

				audioFile.IncreaseOffset(chunkSize);
				audioFile.Read(&data32, 4, Endian::Big);
			}
			audioFile.Read(&chunkSize, 4, Endian::Big);
			const uint32_t commChunkEnd = audioFile.GetOffset() + chunkSize;

			audioFile.Read(&formatInfo.channelLayout.count, 2, Endian::Big);
			formatInfo.channelLayout = AudioChannelLayout::DefaultChannelLayout(formatInfo.channelLayout.count);

			audioFile.IncreaseOffset(4);
			audioFile.Read(&formatInfo.bitsPerSample, 2, Endian::Big);
			audioFile.Read(&srBits, 8, Endian::Big);
			this->SampleRateFrom64(srBits, formatInfo);
			audioFile.IncreaseOffset(2);

			if (formType == aifcID)
			{
				audioFile.Read(&data32, 4, Endian::Big);
				this->FormatTagFrom32(data32, formatInfo);
			}
			else
			{
				formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;
			}

			audioFile.SetOffset(commChunkEnd);

			audioFile.Read(&data32, 4, Endian::Big);
			while (data32 != ssndID)
			{
				audioFile.Read(&chunkSize, 4, Endian::Big);
				if (chunkSize % 2 == 1)
				{
					chunkSize += 1;
				}
				if (audioFile.GetOffset() + chunkSize >= audioFile.FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."));
				}

				audioFile.IncreaseOffset(chunkSize);
				audioFile.Read(&data32, 4, Endian::Big);
			}

			formatInfo.bitRate = AudioFormatInfo::CalculateBitrate(formatInfo);
			return formatInfo;
#endif
		}
		AudioBuffer AiffFormat::ReadFile(const File& audioFile)
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
#else
			AudioFormatInfo audioFormatInfo = this->ReadAudioFormatInfo(audioFile);
			audioFormatInfo.endian = Endian::Big;
			if (audioFormatInfo.formatTag == AIFC_SOWT)
			{
				audioFormatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;
				audioFormatInfo.endian = Endian::Little;
			}

			IAudioCodec* pAudioCodec = AudioCodecManager::FindCodec(audioFormatInfo.formatTag);
			if (pAudioCodec == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat::ReadFile", "Unsupported audio codec."));
			}

			uint32_t audioDataSize = 0;
			audioFile.Read(&audioDataSize, 4, Endian::Big);
			audioFile.IncreaseOffset(8);

			const uint8_t bytesPerSample = audioFormatInfo.bitsPerSample / 8;

			EncodedBufferInfo encodedBufferInfo;
			encodedBufferInfo.pBuffer = malloc(audioDataSize);
			if (encodedBufferInfo.pBuffer == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AiffFormat::ReadFile", "Insufficient memory."));
			}
			audioFile.ReadToBuffer(encodedBufferInfo.pBuffer, bytesPerSample, audioDataSize / bytesPerSample);

			encodedBufferInfo.size_byte = audioDataSize;
			encodedBufferInfo.size_frame = audioDataSize / audioFormatInfo.FrameSize();
			encodedBufferInfo.formatInfo = audioFormatInfo;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(encodedBufferInfo.pBuffer);

			return hephaudioBuffer;
#endif
		}
		AudioBuffer AiffFormat::ReadFile(const File& audioFile, Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying)
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
			const size_t audioDataSize = frameCount * audioFormatInfo.FrameSize();
			uint32_t data32 = 0;

			if (finishedPlaying != nullptr)
			{
				(*finishedPlaying) = false;
			}

			const uint32_t totalFrameCount = this->FileFrameCount(audioFile, audioFormatInfo);

			if (frameIndex > totalFrameCount)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat::ReadFile", "Frame index out of bounds."));
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
			encodedBufferInfo.pBuffer = malloc(audioDataSize);
			if (encodedBufferInfo.pBuffer == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AiffFormat::ReadFile", "Insufficient memory."));
			}
			audioFile.ReadToBuffer(encodedBufferInfo.pBuffer, bytesPerSample, audioDataSize / bytesPerSample);

			encodedBufferInfo.size_byte = audioDataSize;
			encodedBufferInfo.size_frame = audioDataSize / audioFormatInfo.FrameSize();
			encodedBufferInfo.formatInfo = audioFormatInfo;
			encodedBufferInfo.formatInfo.endian = audioFormatInfo.formatTag == AIFC_SOWT ? Endian::Little : Endian::Big;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(encodedBufferInfo.pBuffer);

			return hephaudioBuffer;
#endif
		}
		bool AiffFormat::SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite)
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
				uint32_t data32 = 0, compressionType = 0;
				std::string compressionName;

				audioFile.Write(&formID, 4, Endian::Big);
				data32 = 4;
				audioFile.Write(&data32, 4, Endian::Big);
				audioFile.Write(&aifcID, 4, Endian::Big);

				audioFile.Write(&fverID, 4, Endian::Big);
				audioFile.Write(&data32, 4, Endian::Big);
				audioFile.Write(&aifc_v1, 4, Endian::Big);

				audioFile.Write(&commID, 4, Endian::Big);
				this->FormatTagTo32(bufferFormatInfo, compressionType, compressionName);
				data32 = 22 + compressionName.size();
				audioFile.Write(&data32, 4, Endian::Big);

				audioFile.Write(&bufferFormatInfo.channelLayout.count, 2, Endian::Big);

				data32 = buffer.FrameCount();
				audioFile.Write(&data32, 4, Endian::Big);
				audioFile.Write(&bufferFormatInfo.bitsPerSample, 2, Endian::Big);

				uint64_t srBits = this->SampleRateTo64(bufferFormatInfo);
				audioFile.Write(&srBits, 8, Endian::Big);
				data32 = 0;
				audioFile.Write(&data32, 2, Endian::Big);

				audioFile.Write(&compressionType, 4, Endian::Big);
				audioFile.Write(compressionName.c_str(), compressionName.size(), HEPH_SYSTEM_ENDIAN);

				audioFile.Write(&ssndID, 4, Endian::Big);
				data32 = buffer.Size() + 8;
				audioFile.Write(&data32, 4, Endian::Big);
				data32 = 0;
				audioFile.Write(&data32, 4, Endian::Big);
				audioFile.Write(&data32, 4, Endian::Big);

				if (bufferFormatInfo.endian == Endian::Little)
				{
					AudioProcessor::ChangeEndian(buffer);
				}

				audioFile.WriteFromBuffer(buffer.Begin(), bufferFormatInfo.bitsPerSample / 8, buffer.FrameCount() * bufferFormatInfo.channelLayout.count);
			}
			catch (HephException)
			{
				return false;
			}

			return true;
#endif
		}
		void AiffFormat::SampleRateFrom64(uint64_t srBits, AudioFormatInfo& formatInfo) const
		{
			if (srBits == 0x400FAC4400000000)
			{
				formatInfo.sampleRate = 88200;
			}
			else if (srBits == 0x400EAC4400000000)
			{
				formatInfo.sampleRate = 44100;
			}
			else if (srBits == 0x400DAC4400000000)
			{
				formatInfo.sampleRate = 22050;
			}
			else if (srBits == 0x400CAC4400000000)
			{
				formatInfo.sampleRate = 11025;
			}
			else if (srBits == 0x400FBB8000000000)
			{
				formatInfo.sampleRate = 96000;
			}
			else if (srBits == 0x400EBB8000000000)
			{
				formatInfo.sampleRate = 48000;
			}
			else if (srBits == 0x400DBB8000000000)
			{
				formatInfo.sampleRate = 24000;
			}
			else if (srBits == 0x400CBB8000000000)
			{
				formatInfo.sampleRate = 12000;
			}
			else if (srBits == 0x400BBB8000000000)
			{
				formatInfo.sampleRate = 6000;
			}
			else if (srBits == 0x400FFA0000000000)
			{
				formatInfo.sampleRate = 128000;
			}
			else if (srBits == 0x400EFA0000000000)
			{
				formatInfo.sampleRate = 64000;
			}
			else if (srBits == 0x400DFA0000000000)
			{
				formatInfo.sampleRate = 32000;
			}
			else if (srBits == 0x400CFA0000000000)
			{
				formatInfo.sampleRate = 16000;
			}
			else if (srBits == 0x400BFA0000000000)
			{
				formatInfo.sampleRate = 8000;
			}
			else if (srBits == 0x400AFA0000000000)
			{
				formatInfo.sampleRate = 4000;
			}
			else if (srBits == 0x4009FA0000000000)
			{
				formatInfo.sampleRate = 2000;
			}
			else if (srBits == 0x4008FA0000000000)
			{
				formatInfo.sampleRate = 1000;
			}
			else
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat", "Unknown sample rate."));
			}
		}
		uint64_t AiffFormat::SampleRateTo64(const AudioFormatInfo& formatInfo) const
		{
			if (formatInfo.sampleRate == 88200)
			{
				return 0x400FAC4400000000;
			}
			else if (formatInfo.sampleRate == 44100)
			{
				return 0x400EAC4400000000;
			}
			else if (formatInfo.sampleRate == 22050)
			{
				return 0x400DAC4400000000;
			}
			else if (formatInfo.sampleRate == 11025)
			{
				return 0x400CAC4400000000;
			}
			else if (formatInfo.sampleRate == 96000)
			{
				return 0x400FBB8000000000;
			}
			else if (formatInfo.sampleRate == 48000)
			{
				return 0x400EBB8000000000;
			}
			else if (formatInfo.sampleRate == 24000)
			{
				return 0x400DBB8000000000;
			}
			else if (formatInfo.sampleRate == 12000)
			{
				return 0x400CBB8000000000;
			}
			else if (formatInfo.sampleRate == 6000)
			{
				return 0x400BBB8000000000;
			}
			else if (formatInfo.sampleRate == 128000)
			{
				return 0x400FFA0000000000;
			}
			else if (formatInfo.sampleRate == 64000)
			{
				return 0x400EFA0000000000;
			}
			else if (formatInfo.sampleRate == 32000)
			{
				return 0x400DFA0000000000;
			}
			else if (formatInfo.sampleRate == 16000)
			{
				return 0x400CFA0000000000;
			}
			else if (formatInfo.sampleRate == 8000)
			{
				return 0x400BFA0000000000;
			}
			else if (formatInfo.sampleRate == 4000)
			{
				return 0x400AFA0000000000;
			}
			else if (formatInfo.sampleRate == 2000)
			{
				return 0x4009FA0000000000;
			}
			else if (formatInfo.sampleRate == 1000)
			{
				return 0x4008FA0000000000;
			}
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat", "Unknown sample rate."));
		}
		void AiffFormat::FormatTagFrom32(uint32_t tagBits, AudioFormatInfo& formatInfo) const
		{
			switch (tagBits)
			{
			case 0x4E4F4E45: // "NONE"
				formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_PCM;
				break;
			case 0x736F7774: // "sowt"
				formatInfo.formatTag = AIFC_SOWT;
				break;
			case 0x666C3332: // "fl32"
			case 0x666C3634: // "fl64"
			case 0x464C3332: // "FL32"
				formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT;
				break;
			case 0x414C4157: // "ALAW"
				formatInfo.sampleRate = 8000;
			case 0x616C6177: // "alaw"
				formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_ALAW;
				formatInfo.bitsPerSample = 8;
				break;
			case 0x554C4157: // "ULAW"
				formatInfo.sampleRate = 8000;
			case 0x756C6177: // "ulaw"
				formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_MULAW;
				formatInfo.bitsPerSample = 8;
				break;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat", "Unknown codec."));
			}
		}
		void AiffFormat::FormatTagTo32(const AudioFormatInfo& audioFormatInfo, uint32_t& outTagBits, std::string& outCompressionName) const
		{
			switch (audioFormatInfo.formatTag)
			{
			case HEPHAUDIO_FORMAT_TAG_PCM:
				outTagBits = 0x4E4F4E45; // "NONE"
				outCompressionName = "not compressed";
				break;
			case HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT:
				if (audioFormatInfo.bitsPerSample == 32)
				{
					outTagBits = 0x666C3332; // "fl32"
					outCompressionName = " IEEE 32-bit float";
				}
				else
				{
					outTagBits = 0x666C3634; // "fl64"
					outCompressionName = " IEEE 64-bit float";
				}
				break;
			case HEPHAUDIO_FORMAT_TAG_ALAW:
				outTagBits = 0x616C6177; // "alaw"
				outCompressionName = "ALaw 2:1";
				break;
			case HEPHAUDIO_FORMAT_TAG_MULAW:
				outTagBits = 0x756C6177;
				outCompressionName = "µLaw 2:1";
				break;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AiffFormat", "Codec not supported."));
			}
		}
	}
}