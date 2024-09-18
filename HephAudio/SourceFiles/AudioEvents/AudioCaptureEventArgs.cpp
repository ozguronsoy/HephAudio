#include "AudioEvents/AudioCaptureEventArgs.h"

namespace HephAudio
{
	AudioCaptureEventArgs::AudioCaptureEventArgs(Native::NativeAudio* pNativeAudio, AudioBuffer& captureBuffer) 
		: AudioEventArgs(pNativeAudio), captureBuffer(std::move(captureBuffer)) {}
}