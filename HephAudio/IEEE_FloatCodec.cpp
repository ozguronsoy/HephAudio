#include "IEEE_FloatCodec.h"
#include "File.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace Codecs
	{
		uint32_t IEEE_FloatCodec::Tag() const noexcept
		{
			return WAVE_FORMAT_IEEE_FLOAT;
		}
		AudioBuffer IEEE_FloatCodec::Decode(const EncodedBufferInfo& encodedBufferInfo) const
		{
			AudioBuffer resultBuffer = AudioBuffer(encodedBufferInfo.size_frame, AudioFormatInfo(WAVE_FORMAT_IEEE_FLOAT, encodedBufferInfo.formatInfo.channelCount, sizeof(heph_float) * 8, encodedBufferInfo.formatInfo.sampleRate));

			if (encodedBufferInfo.endian != File::GetSystemEndian())
			{
				if (encodedBufferInfo.formatInfo.bitsPerSample == 32)
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							float ieeSample = ((float*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
							File::ChangeEndian((uint8_t*)&ieeSample, sizeof(float));

							resultBuffer[i][j] = ieeSample;
						}
					}
				}
				else
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							double ieeSample = ((double*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
							File::ChangeEndian((uint8_t*)&ieeSample, sizeof(double));

							resultBuffer[i][j] = ieeSample;
						}
					}
				}
			}
			else
			{
				if (encodedBufferInfo.formatInfo.bitsPerSample == 32)
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = ((float*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
						}
					}
				}
				else
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							resultBuffer[i][j] = ((double*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
						}
					}
				}
			}

			return resultBuffer;
		}
		void IEEE_FloatCodec::Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const
		{
			encodedBufferInfo.size_byte = bufferToEncode.Size();
			encodedBufferInfo.size_frame = bufferToEncode.FrameCount();
			encodedBufferInfo.formatInfo.formatTag = WAVE_FORMAT_IEEE_FLOAT;
			encodedBufferInfo.formatInfo.channelCount = bufferToEncode.FormatInfo().channelCount;
			encodedBufferInfo.formatInfo.bitsPerSample = bufferToEncode.FormatInfo().bitsPerSample;
			encodedBufferInfo.formatInfo.sampleRate = bufferToEncode.FormatInfo().sampleRate;
			AudioBuffer resultBuffer = AudioBuffer(encodedBufferInfo.size_frame, encodedBufferInfo.formatInfo);

			if (encodedBufferInfo.endian != File::GetSystemEndian())
			{
				if (encodedBufferInfo.formatInfo.bitsPerSample == 32)
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							float hephaudioSample = bufferToEncode[i][j];
							File::ChangeEndian((uint8_t*)&hephaudioSample, sizeof(float));

							((float*)resultBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = hephaudioSample;
						}
					}
				}
				else
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							double hephaudioSample = bufferToEncode[i][j];
							File::ChangeEndian((uint8_t*)&hephaudioSample, sizeof(double));

							((double*)resultBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = hephaudioSample;
						}
					}
				}
			}
			else
			{
				if (encodedBufferInfo.formatInfo.bitsPerSample == 32)
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((float*)resultBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = bufferToEncode[i][j];
						}
					}
				}
				else
				{
					for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
					{
						for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
						{
							((double*)resultBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = bufferToEncode[i][j];
						}
					}
				}
			}

			bufferToEncode = std::move(resultBuffer);
		}
	}
}