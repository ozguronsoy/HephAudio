#pragma once
#include "framework.h"
#include "IAudioObject.h"

namespace HephAudio
{
	namespace Structs
	{
		struct AudioObject : public IAudioObject
		{
			AudioBuffer buffer;
			// Starting frame to get sub buffer before the next render.
			size_t frameIndex;
			// There can be more than one queue, if empty the audio object is not in queue.
			std::wstring queueName;
			// Position of the audio object on the queue, if equals to 0 its currently playing.
			uint32_t queueIndex;
			// In milliseconds.
			uint32_t queueDelay;
			AudioObject();
			virtual bool IsPlaying() const override;
			bool IsInQueue() const;
			uint32_t ReversedFrameIndex() const;
			virtual ~AudioObject() = default;
		};
	}
}