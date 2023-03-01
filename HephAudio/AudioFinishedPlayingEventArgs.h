#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include <cinttypes>

namespace HephAudio
{
	struct AudioFinishedPlayingEventArgs : public AudioEventArgs
	{
	public:
		uint32_t remainingLoopCount;
	public:
		AudioFinishedPlayingEventArgs(const void* pNativeAudio, void* pAudioObject, uint32_t remainingLoopCount);
		virtual ~AudioFinishedPlayingEventArgs() = default;
	};
}