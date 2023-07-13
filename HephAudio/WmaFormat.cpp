#include "WmaFormat.h"
#include "HephException.h"
#include "AudioCodecManager.h"
#include "AudioProcessor.h"

using namespace HephAudio::Codecs;

#define ASF_HEADER_OBJECT_ID 0x75B22630, 0x668E, 0x11CF, 0xA6D9, 0x00AA, 0x0062CE6C
#define ASF_DATA_OBJECT_ID 0x75B22636, 0x668E, 0x11CF, 0xA6D9, 0x00AA, 0x0062CE6C
#define ASF_FILE_PROPERTIES_OBJECT_ID 0x8CABDCA1, 0xA947, 0x11CF, 0x8EE4, 0x00C0, 0x0C205365
#define ASF_STREAM_PROPERTIES_OBJECT_ID 0xB7DC0791, 0xA9B7, 0x11CF, 0x8EE6, 0x00C0, 0x0C205365
#define ASF_CODEC_LIST_OBJECT_ID 0x86D15240, 0x311D, 0x11D0, 0xA3A4, 0x00A0, 0xC90348F6
#define ASF_AUDIO_MEDIA_OBJECT_ID 0xF8699E40, 0x5B4D, 0x11CF, 0xA8FD, 0x0080, 0x5F5C442B

namespace HephAudio
{
	namespace FileFormats
	{
		HephCommon::StringBuffer WmaFormat::Extension() const
		{
			return ".wma";
		}
		size_t WmaFormat::FileFrameCount(const HephCommon::File* pAudioFile, const AudioFormatInfo& audioFormatInfo) const
		{
			uint64_t objectSize;
			pAudioFile->SetOffset(30);
			while (!this->CheckGuid(pAudioFile, ASF_FILE_PROPERTIES_OBJECT_ID))
			{
				pAudioFile->Read(&objectSize, 8, Endian::Little);
				if (pAudioFile->GetOffset() + objectSize >= pAudioFile->FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WmaFormat::FileFrameCount", "Failed to read the file. File might be corrupted."))
				}
				pAudioFile->IncreaseOffset(objectSize - 24);
			}
			pAudioFile->IncreaseOffset(48);

			uint64_t playDuration_100ns;
			pAudioFile->Read(&playDuration_100ns, 8, Endian::Little);
			double playDuration_s = playDuration_100ns * 1e-7;

			pAudioFile->IncreaseOffset(8);

			uint64_t preroll_ms;
			pAudioFile->Read(&preroll_ms, 8, Endian::Little);

			return (playDuration_s - preroll_ms * 1e-3) * audioFormatInfo.sampleRate;
		}
		AudioFormatInfo WmaFormat::ReadAudioFormatInfo(const HephCommon::File* pAudioFile) const
		{
			AudioFormatInfo formatInfo;
			uint64_t objectSize, headerObjectSize;

			pAudioFile->SetOffset(0);

			if (!this->CheckGuid(pAudioFile, ASF_HEADER_OBJECT_ID))
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_not_implemented, "WmaFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."))
			}
			pAudioFile->Read(&headerObjectSize, 8, Endian::Little);
			pAudioFile->IncreaseOffset(6); // skip the header count and the reserved bytes

			while (!this->CheckGuid(pAudioFile, ASF_STREAM_PROPERTIES_OBJECT_ID))
			{
				pAudioFile->Read(&objectSize, 8, Endian::Little);
				if (pAudioFile->GetOffset() + objectSize >= pAudioFile->FileSize())
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WmaFormat::ReadAudioFormatInfo", "Failed to read the file. File might be corrupted."))
				}
				pAudioFile->IncreaseOffset(objectSize - 24);
			}

			pAudioFile->IncreaseOffset(8); // skip the object size (QWORD)
			if (!this->CheckGuid(pAudioFile, ASF_AUDIO_MEDIA_OBJECT_ID))
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WmaFormat::ReadAudioFormatInfo", "Unsupported media type."));
			}
			pAudioFile->IncreaseOffset(24); // skip the error correction type (GUID) and the time offset (QWORD)

			uint32_t typeSpecificDataLength;
			pAudioFile->Read(&typeSpecificDataLength, 4, Endian::Little);
			pAudioFile->IncreaseOffset(10); // skip to the type specific data

			pAudioFile->Read(&formatInfo.formatTag, 2, Endian::Little);
			pAudioFile->Read(&formatInfo.channelCount, 2, Endian::Little);
			pAudioFile->Read(&formatInfo.sampleRate, 4, Endian::Little);
			pAudioFile->IncreaseOffset(6);
			pAudioFile->Read(&formatInfo.bitsPerSample, 2, Endian::Little);

			pAudioFile->SetOffset(headerObjectSize);

			return formatInfo;
		}
		AudioBuffer WmaFormat::ReadFile(const HephCommon::File* pAudioFile) const
		{
			const AudioFormatInfo formatInfo = this->ReadAudioFormatInfo(pAudioFile);

			IAudioCodec* pAudioCodec = AudioCodecManager::FindCodec(formatInfo.formatTag);
			if (pAudioCodec == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WmaFormat::ReadFile", "Unsupported audio codec."));
			}

			if (!this->CheckGuid(pAudioFile, ASF_DATA_OBJECT_ID))
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "WmaFormat::ReadFile", "Failed to read the file. File might be corrupted."));
			}

			uint64_t dataObjectSize, totalDataPackets;
			pAudioFile->Read(&dataObjectSize, 8, Endian::Little);
			pAudioFile->IncreaseOffset(16);
			pAudioFile->Read(&totalDataPackets, 8, Endian::Little);
			pAudioFile->IncreaseOffset(2);

			EncodedBufferInfo encodedBufferInfo;
			
			encodedBufferInfo.pBuffer = malloc(dataObjectSize - 50);
			if (encodedBufferInfo.pBuffer == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_insufficient_memory, "WmaFormat::ReadFile", "Insufficient memory."));
			}
			pAudioFile->ReadToBuffer(encodedBufferInfo.pBuffer, 1, dataObjectSize);

			encodedBufferInfo.size_byte = dataObjectSize - 50;
			encodedBufferInfo.size_frame = totalDataPackets;
			encodedBufferInfo.formatInfo = formatInfo;
			encodedBufferInfo.endian = Endian::Little;

			const AudioBuffer hephaudioBuffer = pAudioCodec->Decode(encodedBufferInfo);

			free(encodedBufferInfo.pBuffer);

			return hephaudioBuffer;
		}
		AudioBuffer WmaFormat::ReadFile(const HephCommon::File* pAudioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) const
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_not_implemented, "WmaFormat::ReadFile", "Not implemented."));
		}
		bool WmaFormat::SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_not_implemented, "WmaFormat::SaveToFile", "Not implemented."));
		}
		bool WmaFormat::CheckGuid(const HephCommon::File* pAudioFile, uint32_t d0, uint16_t d1, uint16_t d2, uint16_t d3, uint16_t d4, uint32_t d5)
		{
			bool result = true;
			uint32_t data32;
			uint16_t data16;

			pAudioFile->Read(&data32, 4, Endian::Little);
			if (data32 != d0)
			{
				result = false;
			}

			pAudioFile->Read(&data16, 2, Endian::Little);
			if (data16 != d1)
			{
				result = false;
			}

			pAudioFile->Read(&data16, 2, Endian::Little);
			if (data16 != d2)
			{
				result = false;
			}

			pAudioFile->Read(&data16, 2, Endian::Big);
			if (data16 != d3)
			{
				result = false;
			}

			pAudioFile->Read(&data16, 2, Endian::Big);
			if (data16 != d4)
			{
				result = false;
			}

			pAudioFile->Read(&data32, 4, Endian::Big);
			if (data32 != d5)
			{
				result = false;
			}

			return result;
		}
	}
}