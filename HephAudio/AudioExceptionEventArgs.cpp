#include "AudioExceptionEventArgs.h"

namespace HephAudio
{
	AudioExceptionEventArgs::AudioExceptionEventArgs() : AudioExceptionEventArgs(nullptr, AudioException(), AudioExceptionThread::MainThread) { }
	AudioExceptionEventArgs::AudioExceptionEventArgs(const void* pNativeAudio, AudioException exception, AudioExceptionThread thread) : AudioEventArgs(pNativeAudio, nullptr)
		, exception(exception), thread(thread) { }
}