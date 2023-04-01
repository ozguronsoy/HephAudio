#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "HephException.h"

namespace HephAudio
{
	enum class AudioExceptionThread : uint8_t
	{
		MainThread = 0x01,
		RenderThread = 0x02,
		CaptureThread = 0x04,
		DeviceThread = 0x08,
		QueueThread = 0x016,
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
		HephCommon::HephException exception;
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
		AudioExceptionEventArgs(const void* pNativeAudio, HephCommon::HephException exception, AudioExceptionThread thread);
	};
}