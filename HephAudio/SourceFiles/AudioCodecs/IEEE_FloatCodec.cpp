#include "AudioCodecs/IEEE_FloatCodec.h"
#include <memory>

using namespace HephCommon;

namespace HephAudio
{
	namespace Codecs
	{
		uint32_t IEEE_FloatCodec::Tag() const
		{
			return HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT;
		}
		AudioBuffer IEEE_FloatCodec::Decode(const EncodedBufferInfo& encodedBufferInfo) const
		{
			AudioBuffer resultBuffer(encodedBufferInfo.size_frame, HEPHAUDIO_INTERNAL_FORMAT(encodedBufferInfo.formatInfo.channelCount, encodedBufferInfo.formatInfo.sampleRate));

			for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
			{
				for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
				{
					double sample;
					if (encodedBufferInfo.formatInfo.bitsPerSample == 32)
					{
						sample = ((float*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
						if (encodedBufferInfo.formatInfo.endian != HEPH_SYSTEM_ENDIAN)
						{
							HephCommon::ChangeEndian((uint8_t*)&sample, sizeof(float));
						}
					}
					else
					{
						sample = ((double*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
						if (encodedBufferInfo.formatInfo.endian != HEPH_SYSTEM_ENDIAN)
						{
							HephCommon::ChangeEndian((uint8_t*)&sample, sizeof(double));
						}
					}
					
					resultBuffer[i][j] = sample * HEPH_AUDIO_SAMPLE_MAX;
				}
			}

			return resultBuffer;
		}
		void IEEE_FloatCodec::Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const
		{
			encodedBufferInfo.size_frame = bufferToEncode.FrameCount();
			AudioBuffer resultBuffer(encodedBufferInfo.size_frame, encodedBufferInfo.formatInfo);

			const double scaleFactor = 1.0 / HEPH_AUDIO_SAMPLE_MAX;

			for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
			{
				for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
				{
					if (encodedBufferInfo.formatInfo.bitsPerSample == 32)
					{
						float encodedSample = bufferToEncode[i][j] * scaleFactor;
						if (encodedBufferInfo.formatInfo.endian != HEPH_SYSTEM_ENDIAN)
						{
							HephCommon::ChangeEndian((uint8_t*)&encodedSample, sizeof(float));
						}
						((float*)resultBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = encodedSample;
					}
					else
					{
						double encodedSample = bufferToEncode[i][j] * scaleFactor;
						if (encodedBufferInfo.formatInfo.endian != HEPH_SYSTEM_ENDIAN)
						{
							HephCommon::ChangeEndian((uint8_t*)&encodedSample, sizeof(double));
						}
						((double*)resultBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = encodedSample;
					}
				}
			}

			bufferToEncode = std::move(resultBuffer);
		}
	}
}