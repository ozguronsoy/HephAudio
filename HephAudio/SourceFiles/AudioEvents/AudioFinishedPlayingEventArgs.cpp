#include "AudioEvents/AudioFinishedPlayingEventArgs.h"

namespace HephAudio
{
	AudioFinishedPlayingEventArgs::AudioFinishedPlayingEventArgs(Native::NativeAudio* pNativeAudio, AudioObject* pAudioObject, uint32_t remainingLoopCount)
		: AudioEventArgs(pNativeAudio), pAudioObject(pAudioObject), remainingLoopCount(remainingLoopCount) {}
}