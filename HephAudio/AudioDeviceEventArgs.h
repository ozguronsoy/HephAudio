#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioDevice.h"

namespace HephAudio
{
	/// <summary>
	/// Holds the necessary information to handle an audio device event.
	/// </summary>
	struct AudioDeviceEventArgs : public AudioEventArgs
	{
		/// <summary>
		/// The audio device that triggered the event.
		/// </summary>
		AudioDevice audioDevice;
		/// <summary>
		/// Creates and initializes an AudioDeviceEventArgs instance.
		/// </summary>
		/// <param name="pNativeAudio">Pointer to the NativeAudio instance where the event occurred.</param>
		/// <param name="audioDevice">The audio device that triggered the event.</param>
		AudioDeviceEventArgs(const void* pNativeAudio, AudioDevice audioDevice);
	};
}