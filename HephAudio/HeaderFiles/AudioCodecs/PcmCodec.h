#pragma once
#include "HephAudioShared.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	namespace Codecs
	{
		class PcmCodec final : public IAudioCodec
		{
		public:
			uint32_t Tag() override;
			AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) override;
			void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) override;
		private:
			double GetScaleFactor(const AudioFormatInfo& formatInfo) const;
			double ReadSample(uint8_t* pData, uint16_t bytesPerSample, HephCommon::Endian endian) const;
			void WriteSample(uint8_t* pSample, double value, uint16_t bytesPerSample, HephCommon::Endian endian) const;
		};
	}
}