#pragma once
#include "HephAudioShared.h"
#include "AudioEventArgs.h"

namespace HephAudio
{
	struct AudioFinishedPlayingEventArgs : public AudioEventArgs
	{
		void* pAudioObject;
		uint32_t remainingLoopCount;
		AudioFinishedPlayingEventArgs(void* pAudioObject, void* pNativeAudio, uint32_t remainingLoopCount)
			: AudioEventArgs(pNativeAudio), pAudioObject(pAudioObject), remainingLoopCount(remainingLoopCount) {}
	};
}