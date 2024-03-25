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
			uint32_t Tag() override;
			AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) override;
			void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) override;
		};
	}
}