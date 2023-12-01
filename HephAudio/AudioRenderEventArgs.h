#pragma once
#include "HephAudioFramework.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioRenderEventArgs : public AudioEventArgs
	{
		size_t renderFrameCount;
		AudioRenderEventArgs(const void* pNativeAudio, void* pAudioObject, size_t renderFrameCount);
	};
}