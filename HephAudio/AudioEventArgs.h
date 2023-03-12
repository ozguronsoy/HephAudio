#pragma once
#include "framework.h"

namespace HephAudio
{
	/// <summary>
	/// Holds the necessary information to handle an audio event.
	/// </summary>
	struct AudioEventArgs
	{
		/// <summary>
		/// Pointer to the NativeAudio instance where the event occurred.
		/// </summary>
		void* pNativeAudio;
		/// <summary>
		/// Pointer to the AudioObject instance where the event occurred.
		/// </summary>
		void* pAudioObject;
		/// <summary>
		/// Creates and initializes an AudioEventArgs instance.
		/// </summary>
		/// <param name="pNativeAudio">Pointer to the NativeAudio instance where the event occurred.</param>
		/// <param name="pAudioObject">Pointer to the AudioObject instance where the event occurred.</param>
		AudioEventArgs(const void* pNativeAudio, void* pAudioObject);
		virtual ~AudioEventArgs() = default;
	};
}