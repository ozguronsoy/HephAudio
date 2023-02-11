#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioRenderEventArgs : public AudioEventArgs
	{
	public:
		size_t renderFrameCount;
	public:
		AudioRenderEventArgs(const void* pNativeAudio, void* pAudioObject, size_t renderFrameCount);
		virtual ~AudioRenderEventArgs() = default;
	};
}