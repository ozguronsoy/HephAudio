#include "AudioEvents/AudioFinishedPlayingEventArgs.h"

namespace HephAudio
{
	AudioFinishedPlayingEventArgs::AudioFinishedPlayingEventArgs(Native::NativeAudio* pNativeAudio, AudioObject* pAudioObject)
		: AudioEventArgs(pNativeAudio), pAudioObject(pAudioObject) {}
}