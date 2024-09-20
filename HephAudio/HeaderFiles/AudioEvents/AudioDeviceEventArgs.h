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
	struct HEPH_API AudioDeviceEventArgs : public AudioEventArgs
	{
		/**
		 * the device that caused the event to be raised.
		 * 
		 */
		AudioDevice audioDevice;

		/** 
		 * @copydoc constructor
		 * 
		 * @param pNativeAudio @copydetails AudioEventArgs::pNativeAudio
		 * @param audioDevice @copydetails audioDevice
		 */
		AudioDeviceEventArgs(Native::NativeAudio* pNativeAudio, AudioDevice audioDevice);
	};
}