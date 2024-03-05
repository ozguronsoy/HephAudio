#pragma once
#include "HephAudioShared.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioRenderEventArgs : public AudioEventArgs
	{
		void* pAudioObject;
		size_t renderFrameCount;
		AudioRenderEventArgs(void* pAudioObject, void* pNativeAudio, size_t renderFrameCount) 
			: AudioEventArgs(pNativeAudio), pAudioObject(pAudioObject), renderFrameCount(renderFrameCount) {}
	};
}