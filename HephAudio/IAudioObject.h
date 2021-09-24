#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "DistortionInfo.h"
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
			virtual bool IsPlaying() const = 0;
			IAudioObject();
			virtual ~IAudioObject() = default;
		};
	}
}