#include "AudioEvents/AudioRenderEventArgs.h"

namespace HephAudio
{
	AudioRenderEventArgs::AudioRenderEventArgs(Native::NativeAudio* pNativeAudio, AudioObject* pAudioObject, size_t renderFrameCount)
		: AudioEventArgs(pNativeAudio), pAudioObject(pAudioObject), renderFrameCount(renderFrameCount) {}
}