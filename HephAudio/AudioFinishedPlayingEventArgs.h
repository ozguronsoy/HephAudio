#pragma once
#include "HephAudioFramework.h"
#include "AudioEventArgs.h"
#include <cinttypes>

namespace HephAudio
{
	struct AudioFinishedPlayingEventArgs : public AudioEventArgs
	{
		uint32_t remainingLoopCount;
		AudioFinishedPlayingEventArgs(const void* pNativeAudio, void* pAudioObject, uint32_t remainingLoopCount);
	};
}