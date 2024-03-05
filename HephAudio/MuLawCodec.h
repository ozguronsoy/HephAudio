#pragma once
#include "HephAudioShared.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	namespace Codecs
	{
		class MuLawCodec final : public IAudioCodec
		{
		public:
			uint32_t Tag() const override;
			AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) const override;
			void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const override;
		private:
			int16_t MuLawToPcm(uint8_t mulawSample) const;
			int16_t FindSegment(uint16_t pcmSample) const;
		};
	}
}