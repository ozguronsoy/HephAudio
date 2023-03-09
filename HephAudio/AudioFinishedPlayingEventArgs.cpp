#include "AudioFinishedPlayingEventArgs.h"

namespace HephAudio
{
	AudioFinishedPlayingEventArgs::AudioFinishedPlayingEventArgs(const void* pNativeAudio, void* pAudioObject, uint32_t remainingLoopCount) : AudioEventArgs(pNativeAudio, pAudioObject)
		, remainingLoopCount(remainingLoopCount) { }
}