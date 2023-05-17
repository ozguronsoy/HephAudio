#pragma once
#include "HephAudioFramework.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	namespace Codecs
	{
		class PcmCodec final : public IAudioCodec
		{
		public:
			uint32_t Tag() const noexcept override;
			AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) const override;
			void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const override;
		};
	}
}