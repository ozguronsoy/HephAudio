#include "EncodedAudioBuffer.h"

using namespace HephCommon;

namespace HephAudio
{
	EncodedAudioBuffer::EncodedAudioBuffer() : BufferBase<EncodedAudioBuffer, uint8_t>() {}

	EncodedAudioBuffer::EncodedAudioBuffer(size_t size) : BufferBase<EncodedAudioBuffer, uint8_t>(size) {}

	EncodedAudioBuffer::EncodedAudioBuffer(size_t size, BufferFlags flags) : BufferBase<EncodedAudioBuffer, uint8_t>(size, flags) {}
}