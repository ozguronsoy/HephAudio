#pragma once
#include "HephAudioShared.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	namespace Codecs
	{
		class IEEE_FloatCodec final : public IAudioCodec
		{
		public:
			uint32_t Tag() const override;
			AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) const override;
			void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const override;
		};
	}
}