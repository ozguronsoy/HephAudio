#include "AudioEventArgs.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioEventArgs::AudioEventArgs(const void* pNativeAudio, void* pAudioObject)
		: EventArgs(pNativeAudio), pNativeAudio(const_cast<void*>(pNativeAudio)), pAudioObject(pAudioObject) { }
}