#include "AudioCaptureEventArgs.h"
#include <memory>

namespace HephAudio
{
	AudioCaptureEventArgs::AudioCaptureEventArgs(const void* pNativeAudio, AudioBuffer& captureBuffer) : AudioEventArgs(pNativeAudio, this) 
	{
		this->captureBuffer = std::move(captureBuffer);
	}
}