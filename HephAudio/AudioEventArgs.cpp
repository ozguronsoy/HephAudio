#include "AudioEventArgs.h"

namespace HephAudio
{
	AudioEventArgs::AudioEventArgs(const void* pNativeAudio, void* pAudioObject)
		: pNativeAudio(const_cast<void*>(pNativeAudio)), pAudioObject(pAudioObject) { }
}