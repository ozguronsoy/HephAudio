#pragma once
#include "HephAudioShared.h"
#include "AudioEventArgs.h"
#include "AudioDevice.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief struct for storing the arguments for the audio device events.
	 * 
	 */
	struct AudioDeviceEventArgs : public AudioEventArgs
	{
		/**
		 * the device that caused the event to be raised.
		 * 
		 */
		AudioDevice audioDevice;

		/**
		 * creates a new instance and initializes it with the provided values.
		 * 
		 */
		AudioDeviceEventArgs(void* pNativeAudio, AudioDevice audioDevice) : AudioEventArgs(pNativeAudio), audioDevice(audioDevice) {}
	};
}