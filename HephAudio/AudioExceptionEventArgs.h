#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioException.h"

namespace HephAudio
{
	enum class AudioExceptionThread : uint8_t
	{
		MainThread = 0x00,
		RenderThread = 0x01,
		CaptureThread = 0x02,
		QueueThread = 0x04,
		Other = 0xFF
	};
	struct AudioExceptionEventArgs : public AudioEventArgs
	{
	public:
		AudioException exception;
		AudioExceptionThread thread;
	public:
		AudioExceptionEventArgs();
		AudioExceptionEventArgs(const void* pNativeAudio, AudioException exception, AudioExceptionThread thread);
		virtual ~AudioExceptionEventArgs() = default;
	};
}