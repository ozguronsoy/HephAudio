#include "AudioCodecs/PcmCodec.h"
#include "../HephCommon/HeaderFiles/HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace Codecs
	{
		uint32_t PcmCodec::Tag()
		{
			return HEPHAUDIO_FORMAT_TAG_PCM;
		}
		AudioBuffer PcmCodec::Decode(const EncodedBufferInfo& encodedBufferInfo)
		{
			AudioBuffer resultBuffer = AudioBuffer(encodedBufferInfo.size_frame, HEPHAUDIO_INTERNAL_FORMAT(encodedBufferInfo.formatInfo.channelCount, encodedBufferInfo.formatInfo.sampleRate));
			const double scaleFactor = PcmCodec::GetScaleFactor(encodedBufferInfo.formatInfo);
			const uint16_t frameSize = encodedBufferInfo.formatInfo.FrameSize();
			const uint16_t bytesPerSample = encodedBufferInfo.formatInfo.bitsPerSample / 8;

			for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
			{
				for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
				{
					uint8_t* pSample = (uint8_t*)encodedBufferInfo.pBuffer + i * frameSize + j * bytesPerSample;
					const double floatSample = PcmCodec::ReadSample(pSample, bytesPerSample, encodedBufferInfo.formatInfo.endian);
					resultBuffer[i][j] = floatSample * scaleFactor;
				}
			}

			return resultBuffer;
		}
		void PcmCodec::Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo)
		{
			encodedBufferInfo.size_frame = bufferToEncode.FrameCount();

			AudioBuffer tempBuffer(bufferToEncode.FrameCount(), encodedBufferInfo.formatInfo);
			const double scaleFactor = 1.0 / PcmCodec::GetScaleFactor(encodedBufferInfo.formatInfo);
			const uint16_t frameSize = encodedBufferInfo.formatInfo.FrameSize();
			const uint16_t bytesPerSample = encodedBufferInfo.formatInfo.bitsPerSample / 8;

			for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
			{
				for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
				{
					uint8_t* pSample = ((uint8_t*)tempBuffer.Begin()) + i * frameSize + j * bytesPerSample;
					WriteSample(pSample, bufferToEncode[i][j] * scaleFactor, bytesPerSample, encodedBufferInfo.formatInfo.endian);
				}
			}

			bufferToEncode = std::move(tempBuffer);
		}
		double PcmCodec::GetScaleFactor(const AudioFormatInfo& formatInfo) const
		{
			switch (formatInfo.bitsPerSample)
			{
			case 8:
				return ((double)HEPH_AUDIO_SAMPLE_MAX) / UINT8_MAX;
			case 16:
				return ((double)HEPH_AUDIO_SAMPLE_MAX) / (INT16_MAX + 1.0);
			case 24:
				return ((double)HEPH_AUDIO_SAMPLE_MAX) / (INT24_MAX + 1.0);
			case 32:
				return ((double)HEPH_AUDIO_SAMPLE_MAX) / (INT32_MAX + 1.0);
			case 64:
				return ((double)HEPH_AUDIO_SAMPLE_MAX) / (INT64_MAX + 1.0);
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HEPH_EC_FAIL, "PcmCodec", "Invalid sample size."));
			}
		}
		double PcmCodec::ReadSample(uint8_t* pData, uint16_t bytesPerSample, Endian endian) const
		{
			switch (bytesPerSample)
			{
			case 1:
				return *(uint8_t*)pData + INT8_MIN;
			case 2:
			{
				int16_t sample = *(int16_t*)pData;
				if (endian != HEPH_SYSTEM_ENDIAN)
				{
					HephCommon::ChangeEndian((uint8_t*)&sample, 2);
				}
				return sample;
			}
			case 3:
			{
				int32_t sample = (pData[0] << 16) | (pData[1] << 8) | pData[2];
				if (endian != Endian::Big)
				{
					HephCommon::ChangeEndian((uint8_t*)&sample, 3);
				}

				if (sample & 0x800000)
				{
					sample ^= 0xFFFFFF;
					return -((double)(sample + 1));
				}

				return sample;
			}
			case 4:
			{
				int32_t sample = *(int32_t*)pData;
				if (endian != HEPH_SYSTEM_ENDIAN)
				{
					HephCommon::ChangeEndian((uint8_t*)&sample, 4);
				}
				return sample;
			}
			case 8:
			{
				int64_t sample = *(int64_t*)pData;
				if (endian != HEPH_SYSTEM_ENDIAN)
				{
					HephCommon::ChangeEndian((uint8_t*)&sample, 8);
				}
				return sample;
			}
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HEPH_EC_FAIL, "PcmCodec", "Invalid sample size."));
			}
		}
		void PcmCodec::WriteSample(uint8_t* pSample, double value, uint16_t bytesPerSample, Endian endian) const
		{
			switch (bytesPerSample)
			{
			case 1:
				(*pSample) = (value + UINT8_MAX) / 2;
				break;
			case 2:
			{
				int16_t* pTemp = (int16_t*)pSample;
				(*pTemp) = value;
				if (endian != HEPH_SYSTEM_ENDIAN)
				{
					HephCommon::ChangeEndian((uint8_t*)pTemp, 2);
				}
			}
			break;
			case 3:
			{
				int32_t temp = ((int32_t)value) << 8;
				*(pSample) = temp >> 24;
				*(pSample + 1) = temp >> 16;
				*(pSample + 2) = temp >> 8;
				if (endian != Endian::Big)
				{
					HephCommon::ChangeEndian(pSample, 3);
				}
			}
			break;
			case 4:
			{
				int32_t* pTemp = (int32_t*)pSample;
				(*pTemp) = value;
				if (endian != HEPH_SYSTEM_ENDIAN)
				{
					HephCommon::ChangeEndian((uint8_t*)pTemp, 4);
				}
			}
			break;
			case 8:
			{
				int64_t* pTemp = (int64_t*)pSample;
				(*pTemp) = value;
				if (endian != HEPH_SYSTEM_ENDIAN)
				{
					HephCommon::ChangeEndian((uint8_t*)pTemp, 8);
				}
			}
			break;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HEPH_EC_FAIL, "PcmCodec", "Invalid sample size."));
			}
		}
	}
}