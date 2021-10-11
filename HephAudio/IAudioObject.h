#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "DistortionInfo.h"
#include "EchoInfo.h"
#include <string>
#include <vector>

namespace HephAudio
{
	namespace Structs
	{
		struct IAudioObject
		{
			std::wstring filePath;
			std::wstring name;
			bool paused;
			bool mute;
			bool reverse;
			// A constant object does not finish playing until you either destroy it by calling the INativeAudio::DestroyAO method or make this false and wait until it finishes.
			bool constant;
			uint32_t loopCount; // Set this to 0 for an infinite loop.
			double volume; // Setting the volume to more than 1 might cause some glitching in audio.
			std::vector<std::wstring> categories;
			DistortionInfo distortionInfo;
			EchoInfo echoInfo;
			AudioBuffer buffer;
			// Starting frame to get sub buffer before the next render.
			size_t frameIndex;
			// There can be more than one queue, if empty the audio object is not in queue.
			std::wstring queueName;
			// Position of the audio object on the queue, if equals to 0 its currently playing.
			uint32_t queueIndex;
			// In milliseconds.
			uint32_t queueDelay;
			virtual bool IsPlaying() const;
			virtual bool IsInQueue() const;
			virtual size_t ReversedFrameIndex() const;
			virtual size_t FrameCount() const;
			IAudioObject();
			virtual ~IAudioObject() = default;
		};
	}
}