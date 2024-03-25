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
			virtual uint32_t Tag() = 0;
			virtual AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo) = 0;
			virtual void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo) = 0;
		};
	}
}