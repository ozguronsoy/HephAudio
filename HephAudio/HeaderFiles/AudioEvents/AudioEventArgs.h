#pragma once
#include "HephAudioShared.h"
#include "EventArgs.h"
#include "NativeAudio/NativeAudio.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for audio event arguments.
	 * 
	 */
	struct HEPH_API AudioEventArgs : public Heph::EventArgs
	{
		/**
		 * pointer to the native audio instance that raised the event.
		 * 
		 */
		Native::NativeAudio* pNativeAudio;

		/** 
		 * @copydoc constructor
		 * 
		 * @param pNativeAudio @copydetails pNativeAudio
		 */
		AudioEventArgs(Native::NativeAudio* pNativeAudio);

		/** @copydoc destructor */
		virtual ~AudioEventArgs() = default;
	};
}