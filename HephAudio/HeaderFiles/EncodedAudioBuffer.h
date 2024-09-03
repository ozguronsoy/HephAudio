#pragma once
#include "HephAudioShared.h"
#include "Buffers/BufferBase.h"

namespace HephAudio
{
	class EncodedAudioBuffer : protected HephCommon::BufferBase<EncodedAudioBuffer, uint8_t>
	{
	public:
		EncodedAudioBuffer();
		explicit EncodedAudioBuffer(size_t size);
		EncodedAudioBuffer(size_t size, HephCommon::BufferFlags flags);
		virtual ~EncodedAudioBuffer() = default;
	};
}