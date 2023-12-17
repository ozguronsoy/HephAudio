#include "PcmCodec.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "int24.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace Codecs
	{
		uint32_t PcmCodec::Tag() const noexcept
		{
			return WAVE_FORMAT_PCM;
		}
		AudioBuffer PcmCodec::Decode(const EncodedBufferInfo& encodedBufferInfo) const
		{
			AudioBuffer resultBuffer = AudioBuffer(encodedBufferInfo.size_frame, HEPHAUDIO_INTERNAL_FORMAT(encodedBufferInfo.formatInfo.channelCount, encodedBufferInfo.formatInfo.sampleRate));

			if (encodedBufferInfo.endian != HEPH_SYSTEM_ENDIAN)
			{
				switch (encodedBufferInfo.formatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (heph_float)((uint8_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int16_t pcmSample = ((int16_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
							HephCommon::ChangeEndian((uint8_t*)&pcmSample, 2);

							resultBuffer[i][j] = (heph_float)pcmSample / (INT16_MAX + 1);
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int24 pcmSample = ((int24*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
							HephCommon::ChangeEndian((uint8_t*)&pcmSample, 3);

							resultBuffer[i][j] = (heph_float)pcmSample / (INT24_MAX + 1);
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int32_t pcmSample = ((int32_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
							HephCommon::ChangeEndian((uint8_t*)&pcmSample, 4);

							resultBuffer[i][j] = (heph_float)pcmSample / (INT32_MAX + 1u);
						}
					}
				}
				break;
				default:
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "PcmCodec::Decode", "Invalid sample size."));
				}
			}
			else
			{
				switch (encodedBufferInfo.formatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (heph_float)((uint8_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (heph_float)((int16_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / (INT16_MAX + 1);
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (heph_float)((int24*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / (INT24_MAX + 1);
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = (heph_float)((int32_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j] / (INT32_MAX + 1u);
						}
					}
				}
				break;
				default:
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "PcmCodec::Decode", "Invalid sample size."));
				}
			}

			return resultBuffer;
		}
		void PcmCodec::Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const
		{
			encodedBufferInfo.size_byte = bufferToEncode.Size();
			encodedBufferInfo.size_frame = bufferToEncode.FrameCount();
			encodedBufferInfo.formatInfo.formatTag = WAVE_FORMAT_PCM;
			encodedBufferInfo.formatInfo.channelCount = bufferToEncode.FormatInfo().channelCount;
			encodedBufferInfo.formatInfo.sampleRate = bufferToEncode.FormatInfo().sampleRate;
			AudioBuffer tempBuffer = AudioBuffer(bufferToEncode.FrameCount(), encodedBufferInfo.formatInfo);

			if (encodedBufferInfo.endian != HEPH_SYSTEM_ENDIAN)
			{
				switch (encodedBufferInfo.formatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((uint8_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = (bufferToEncode[i][j] * 0.5 + 0.5) * UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int16_t pcmSample = bufferToEncode[i][j] * INT16_MAX;
							HephCommon::ChangeEndian((uint8_t*)&pcmSample, 2);

							((int16_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int24 pcmSample = bufferToEncode[i][j] * INT24_MAX;
							HephCommon::ChangeEndian((uint8_t*)&pcmSample, 3);

							((int24*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							int32_t pcmSample = bufferToEncode[i][j] * INT32_MAX;
							HephCommon::ChangeEndian((uint8_t*)&pcmSample, 4);

							((int32_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = pcmSample;
						}
					}
				}
				break;
				default:
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "PcmCodec::Encode", "Invalid sample size."));
				}
			}
			else
			{
				switch (encodedBufferInfo.formatInfo.bitsPerSample)
				{
				case 8:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((uint8_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = (bufferToEncode[i][j] * 0.5 + 0.5) * UINT8_MAX;
						}
					}
				}
				break;
				case 16:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((int16_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = bufferToEncode[i][j] * INT16_MAX;
						}
					}
				}
				break;
				case 24:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((int24*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = bufferToEncode[i][j] * INT24_MAX;
						}
					}
				}
				break;
				case 32:
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((int32_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = bufferToEncode[i][j] * INT32_MAX;
						}
					}
				}
				break;
				default:
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "PcmCodec::Encode", "Invalid sample size."));
				}
			}

			bufferToEncode = std::move(tempBuffer);
		}
	}
}