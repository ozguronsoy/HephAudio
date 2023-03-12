#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include <cinttypes>

namespace HephAudio
{
	/// <summary>
	/// Holds the necessary information to handle an audio finished playing event.
	/// </summary>
	struct AudioFinishedPlayingEventArgs : public AudioEventArgs
	{
		/// <summary>
		/// The number of times the AudioObject will be played again.
		/// </summary>
		uint32_t remainingLoopCount;
		/// <summary>
		/// Creates and initializes an AudioFinishedPlayingEventArgs instance.
		/// </summary>
		/// <param name="pNativeAudio">Pointer to the NativeAudio instance where the event occurred.</param>
		/// <param name="pAudioObject">Pointer to the AudioObject instance where the event occurred.</param>
		/// <param name="remainingLoopCount">The number of times the AudioObject will be played again.</param>
		AudioFinishedPlayingEventArgs(const void* pNativeAudio, void* pAudioObject, uint32_t remainingLoopCount);
	};
}