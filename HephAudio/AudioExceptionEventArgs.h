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
	/// <summary>
	/// Holds the necessary information to handle an audio exception event.
	/// </summary>
	struct AudioExceptionEventArgs : public AudioEventArgs
	{
		/// <summary>
		/// The exception that triggered the event
		/// </summary>
		AudioException exception;
		/// <summary>
		/// The thread where the exception occurred.
		/// </summary>
		AudioExceptionThread thread;
		/// <summary>
		/// Creates and initializes an AudioExceptionEventArgs instance with the default values.
		/// </summary>
		AudioExceptionEventArgs();
		/// <summary>
		/// Creates and initializes an AudioExceptionEventArgs instance.
		/// </summary>
		/// <param name="pNativeAudio">Pointer to the NativeAudio instance where the event occurred.</param>
		/// <param name="exception">The exception that triggered the event.</param>
		/// <param name="thread">The thread where the exception occurred.</param>
		AudioExceptionEventArgs(const void* pNativeAudio, AudioException exception, AudioExceptionThread thread);
	};
}