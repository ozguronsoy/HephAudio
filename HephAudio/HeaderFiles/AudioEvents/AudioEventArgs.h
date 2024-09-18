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
		 * creates a new instance and initializes it with the provided values.
		 * 
		 */
		AudioEventArgs(void* pNativeAudio) : pNativeAudio(pNativeAudio) {}

		/**
		 * releases the resources and destroys the instance.
		 * 
		 */
		virtual ~AudioEventArgs() = default;
	};
}