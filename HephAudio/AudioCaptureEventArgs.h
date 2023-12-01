#pragma once
#include "HephAudioFramework.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioCaptureEventArgs : public AudioEventArgs
	{
		AudioBuffer captureBuffer;
		AudioCaptureEventArgs(const void* pNativeAudio, AudioBuffer& captureBuffer);
	};
}