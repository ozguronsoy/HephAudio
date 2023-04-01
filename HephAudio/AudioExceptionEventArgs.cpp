#include "AudioExceptionEventArgs.h"

namespace HephAudio
{
	AudioExceptionEventArgs::AudioExceptionEventArgs() : AudioExceptionEventArgs(nullptr, HephCommon::HephException(), AudioExceptionThread::MainThread) { }
	AudioExceptionEventArgs::AudioExceptionEventArgs(const void* pNativeAudio, HephCommon::HephException exception, AudioExceptionThread thread) : AudioEventArgs(pNativeAudio, nullptr)
		, exception(exception), thread(thread) { }
}