#pragma once
#include "HephAudioShared.h"
#include "AudioEventArgs.h"
#include "AudioObject.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief struct for storing the arguments for the audio finished playing events.
	 * 
	 */
	struct HEPH_API AudioFinishedPlayingEventArgs : public AudioEventArgs
	{
		/**
		 * pointer to the audio object instance that's being rendered.
		 * 
		 */
		AudioObject* pAudioObject;

		/**
		 * number of loops remaining.
		 * Number of times the audio object will be played again.
		 * 
		 */
		uint32_t remainingLoopCount;

		/** 
		 * @copydoc constructor
		 * 
		 * @param pNativeAudio @copydetails AudioEventArgs::pNativeAudio
		 * @param pAudioObject @copydetails pAudioObject
		 * @param remainingLoopCount @copydetails remainingLoopCount
		 */
		AudioFinishedPlayingEventArgs(Native::NativeAudio* pNativeAudio, AudioObject* pAudioObject, uint32_t remainingLoopCount);
	};
}