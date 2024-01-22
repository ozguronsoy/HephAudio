#include "ALawCodec.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

namespace HephAudio
{
	namespace Codecs
	{
		static constexpr int16_t decodeTable[128] =
		{
			-5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736,
			-7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784,
			-2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368,
			-3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392,
			-22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944,
			-30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136,
			-11008, -10496, -12032, -11520, -8960, -8448, -9984, -9472,
			-15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568,
			-344, -328, -376, -360, -280, -264, -312, -296,
			-472, -456, -504, -488, -408, -392, -440, -424,
			-88, -72, -120, -104, -24, -8, -56, -40,
			-216, -200, -248, -232, -152, -136, -184, -168,
			-1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184,
			-1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696,
			-688, -656, -752, -720, -560, -528, -624, -592,
			-944, -912, -1008, -976, -816, -784, -880, -848
		};

		static constexpr uint8_t segmentTable[128] =
		{
			1, 1, 2, 2, 3, 3, 3, 3,
			4, 4, 4, 4, 4, 4, 4, 4,
			5, 5, 5, 5, 5, 5, 5, 5,
			5, 5, 5, 5, 5, 5, 5, 5,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
			6, 6, 6, 6, 6, 6, 6, 6,
			7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7,
		};

		uint32_t ALawCodec::Tag() const
		{
			return HEPHAUDIO_FORMAT_TAG_ALAW;
		}
		AudioBuffer ALawCodec::Decode(const EncodedBufferInfo& encodedBufferInfo) const
		{
			AudioBuffer resultBuffer = AudioBuffer(encodedBufferInfo.size_frame, HEPHAUDIO_INTERNAL_FORMAT(encodedBufferInfo.formatInfo.channelCount, encodedBufferInfo.formatInfo.sampleRate));

			for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
			{
				for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
				{
					const uint8_t encodedSample = ((int8_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelCount + j];
					const int16_t pcmSample = encodedSample < 128 ? decodeTable[encodedSample] : -decodeTable[encodedSample - 128];

					resultBuffer[i][j] = (heph_float)pcmSample / INT16_MAX;
				}
			}

			return resultBuffer;
		}
		void ALawCodec::Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const
		{
			AudioProcessor::ChangeSampleRate(bufferToEncode, 8000);
			encodedBufferInfo.size_byte = bufferToEncode.Size();
			encodedBufferInfo.size_frame = bufferToEncode.FrameCount();
			encodedBufferInfo.formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_ALAW;
			encodedBufferInfo.formatInfo.channelCount = bufferToEncode.FormatInfo().channelCount;
			encodedBufferInfo.formatInfo.bitsPerSample = 8;
			encodedBufferInfo.formatInfo.sampleRate = 8000;
			AudioBuffer tempBuffer = AudioBuffer(encodedBufferInfo.size_frame, encodedBufferInfo.formatInfo);

			for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
			{
				for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelCount; j++)
				{
					int16_t pcmSample = bufferToEncode[i][j] * INT16_MAX;
					uint8_t mask;
					if (pcmSample >= 0)
					{
						mask = 0xD5;
					}
					else
					{
						mask = 0x55;
						pcmSample = HephCommon::Math::Min(-pcmSample, 32767);
					}

					uint8_t compressedByte;
					if (pcmSample > 255)
					{
						int16_t segment = segmentTable[(pcmSample & 0x7F00) >> 8];
						compressedByte = (segment << 4) | ((pcmSample >> (segment + 3)) & 0x0F);
					}
					else
					{
						compressedByte = pcmSample >> 4;
					}

					((uint8_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelCount + j] = compressedByte ^ mask;
				}
			}

			bufferToEncode = std::move(tempBuffer);
		}
	}
}