#include "AudioCodecs/MuLawCodec.h"
#include "AudioProcessor.h"
#include "HephMath.h"

namespace HephAudio
{
	namespace Codecs
	{
		uint32_t MuLawCodec::Tag()
		{
			return HEPHAUDIO_FORMAT_TAG_MULAW;
		}
		AudioBuffer MuLawCodec::Decode(const EncodedBufferInfo& encodedBufferInfo)
		{
			constexpr double scaleFactor = ((double)HEPH_AUDIO_SAMPLE_MAX) / INT16_MAX;

			AudioBuffer resultBuffer = AudioBuffer(encodedBufferInfo.size_frame, HEPHAUDIO_INTERNAL_FORMAT(encodedBufferInfo.formatInfo.channelLayout, encodedBufferInfo.formatInfo.sampleRate));

			for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
			{
				for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelLayout.count; j++)
				{
					const uint8_t encodedSample = ((int8_t*)encodedBufferInfo.pBuffer)[i * encodedBufferInfo.formatInfo.channelLayout.count + j];
					const int16_t pcmSample = MuLawCodec::MuLawToPcm(encodedSample);

					resultBuffer[i][j] = (double)pcmSample * scaleFactor;
				}
			}

			return resultBuffer;
		}
		void MuLawCodec::Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo)
		{
			constexpr double scaleFactor = INT16_MAX / ((double)HEPH_AUDIO_SAMPLE_MAX);

			AudioProcessor::ChangeSampleRate(bufferToEncode, 8000);
			encodedBufferInfo.size_byte = bufferToEncode.Size();
			encodedBufferInfo.size_frame = bufferToEncode.FrameCount();
			encodedBufferInfo.formatInfo.formatTag = HEPHAUDIO_FORMAT_TAG_MULAW;
			encodedBufferInfo.formatInfo.channelLayout = bufferToEncode.FormatInfo().channelLayout;
			encodedBufferInfo.formatInfo.bitsPerSample = 8;
			encodedBufferInfo.formatInfo.sampleRate = 8000;
			AudioBuffer tempBuffer = AudioBuffer(encodedBufferInfo.size_frame, encodedBufferInfo.formatInfo);

			for (size_t i = 0; i < encodedBufferInfo.size_frame; i++)
			{
				for (size_t j = 0; j < encodedBufferInfo.formatInfo.channelLayout.count; j++)
				{
					int16_t pcmSample = bufferToEncode[i][j] * scaleFactor;
					const int8_t sign = (pcmSample & 0x8000) >> 8;
					pcmSample = HephCommon::Math::Min(abs(pcmSample), 32767) + 132;
					const int16_t segment = MuLawCodec::FindSegment((pcmSample & 0x7F80) >> 7);
					((uint8_t*)tempBuffer.Begin())[i * encodedBufferInfo.formatInfo.channelLayout.count + j] = ~(sign | (segment << 4) | ((pcmSample >> (segment + 3)) & 0x0F));
				}
			}

			bufferToEncode = std::move(tempBuffer);
		}
		int16_t MuLawCodec::MuLawToPcm(uint8_t mulawSample) const
		{
			const uint8_t sign = (mulawSample & 0x80) >> 7;
			if (sign != 0)
			{
				mulawSample -= 128;
			}
			const uint8_t division = mulawSample / 16;
			const uint8_t remainder = mulawSample % 16;
			int16_t pcmSample = -32124;

			uint16_t pow2 = 1;
			for (size_t i = 0; i < division; i++, pow2 <<= 1)
			{
				pcmSample += (1024 / pow2) * 15;
				pcmSample += 768 / pow2;
			}
			pcmSample += (1024 / pow2) * remainder;

			return sign == 1 ? -pcmSample : pcmSample;
		}
		int16_t MuLawCodec::FindSegment(uint16_t pcmSample) const
		{
			for (size_t i = 7; i > 0; i--)
			{
				if (((pcmSample >> i) & 0x01) == 1)
				{
					return i;
				}
			}
			return 0;
		}
	}
}