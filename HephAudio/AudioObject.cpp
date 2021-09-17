#include "AudioObject.h"

namespace HephAudio
{
	namespace Structs
	{
		AudioObject::AudioObject() : IAudioObject()
		{
			buffer = AudioBuffer();
			frameIndex = 0;
			queueName = L"";
			queueIndex = 0;
			queueDelay = 0;
		}
		bool AudioObject::IsPlaying() const
		{
			return !paused && !IsInQueue();
		}
		bool AudioObject::IsInQueue() const
		{
			return queueName != L"" && queueIndex > 0;
		}
		uint32_t AudioObject::ReversedFrameIndex() const
		{
			return buffer.FrameCount() - frameIndex - 1u;
		}
	}
}