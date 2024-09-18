#pragma once
#include "HephAudioShared.h"
#include "AudioEventArgs.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief struct for storing the arguments for the audio finished playing events.
	 * 
	 */
	struct AudioFinishedPlayingEventArgs : public AudioEventArgs
	{
		/**
		 * pointer to the audio object instance that's being rendered.
		 * 
		 */
		void* pAudioObject;

		/**
		 * number of loops remaining.
		 * Number of times the audio object will be played again.
		 * 
		 */
		uint32_t remainingLoopCount;

		/**
		 * creates a new instance and initializes it with the provided values.
		 * 
		 */
		AudioFinishedPlayingEventArgs(void* pAudioObject, void* pNativeAudio, uint32_t remainingLoopCount)
			: AudioEventArgs(pNativeAudio), pAudioObject(pAudioObject), remainingLoopCount(remainingLoopCount) {}
	};
}