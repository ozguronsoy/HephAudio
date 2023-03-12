#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	/// <summary>
	/// Holds the necessary information to handle an audio capture event.
	/// </summary>
	struct AudioCaptureEventArgs : public AudioEventArgs
	{
		/// <summary>
		/// The captured audio data.
		/// </summary>
		AudioBuffer captureBuffer;
		/// <summary>
		/// Creates and initializes an AudioCaptureEventArgs instance.
		/// </summary>
		/// <param name="pNativeAudio">Pointer to the NativeAudio instance where the event occurred.</param>
		/// <param name="captureBuffer">The captured audio data.</param>
		AudioCaptureEventArgs(const void* pNativeAudio, AudioBuffer& captureBuffer);
	};
}