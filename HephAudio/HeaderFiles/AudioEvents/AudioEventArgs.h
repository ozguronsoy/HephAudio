#pragma once
#include "HephAudioShared.h"
#include "EventArgs.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for audio event arguments.
	 * 
	 */
	struct AudioEventArgs : public HephCommon::EventArgs
	{
		/**
		 * pointer to the native audio instance that raised the event.
		 * 
		 */
		void* pNativeAudio;

		/** 
		 * @copydoc constructor
		 * 
		 * @param pNativeAudio @copydetails pNativeAudio
		 */
		AudioEventArgs(void* pNativeAudio) : pNativeAudio(pNativeAudio) {}

		/** @copydoc destructor */
		virtual ~AudioEventArgs() = default;
	};
}