#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	/// <summary>
	/// Holds the necessary information to handle an audio render event.
	/// </summary>
	struct AudioRenderEventArgs : public AudioEventArgs
	{
		/// <summary>
		/// The number of frames to be rendered.
		/// </summary>
		size_t renderFrameCount;
		/// <summary>
		/// Creates and initializes an AudioRenderEventArgs instance.
		/// </summary>
		/// <param name="pNativeAudio">Pointer to the NativeAudio instance where the event occurred.</param>
		/// <param name="pAudioObject">Pointer to the AudioObject instance where the event occurred.</param>
		/// <param name="renderFrameCount">The number of frames to be rendered.</param>
		AudioRenderEventArgs(const void* pNativeAudio, void* pAudioObject, size_t renderFrameCount);
	};
}