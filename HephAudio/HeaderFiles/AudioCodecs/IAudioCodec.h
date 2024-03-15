#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "EncodedBufferInfo.h"

namespace HephAudio
{
	namespace Codecs
	{
		class IAudioCodec
		{
		public:
			virtual ~IAudioCodec() = default;
			virtual uint32_t Tag() const = 0;
			virtual AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) const = 0;
			virtual void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) const = 0;
		};
	}
}