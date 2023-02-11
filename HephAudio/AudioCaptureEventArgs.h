#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	struct AudioCaptureEventArgs : public AudioEventArgs
	{
	public:
		AudioBuffer captureBuffer;
	public:
		AudioCaptureEventArgs(const void* pNativeAudio, AudioBuffer& captureBuffer);
		virtual ~AudioCaptureEventArgs() = default;
	};
}