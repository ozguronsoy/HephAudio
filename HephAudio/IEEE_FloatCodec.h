#pragma once
#include "framework.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	namespace AudioCodecs
	{
		class IEEE_FloatCodec final : public IAudioCodec
		{
		public:
			uint32_t Tag() const noexcept override;
			AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) const override;
			void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const override;
		};
	}
}