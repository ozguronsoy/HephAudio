#pragma once
#include "HephAudioShared.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioCaptureEventArgs : public AudioEventArgs
	{
		AudioBuffer captureBuffer;
		AudioCaptureEventArgs(void* pNativeAudio, AudioBuffer& captureBuffer) : AudioEventArgs(pNativeAudio), captureBuffer(std::move(captureBuffer)) {}
	};
}