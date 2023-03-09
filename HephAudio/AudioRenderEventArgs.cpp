#include "AudioRenderEventArgs.h"

namespace HephAudio
{
	AudioRenderEventArgs::AudioRenderEventArgs(const void* pNativeAudio, void* pAudioObject, size_t renderFrameCount) : AudioEventArgs(pNativeAudio, pAudioObject)
		, renderFrameCount(renderFrameCount) { }
}