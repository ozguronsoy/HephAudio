#include "AudioEventArgs.h"

namespace HephAudio
{
	AudioEventArgs::AudioEventArgs(const void* pNativeAudio, void* pAudioObject)
	{
		this->pNativeAudio = const_cast<void*>(pNativeAudio);
		this->pAudioObject = pAudioObject;
	}
}