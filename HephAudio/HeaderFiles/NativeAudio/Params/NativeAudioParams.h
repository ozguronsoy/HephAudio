#pragma once
#include "HephAudioShared.h"

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief base class for storing the native audio API specific parameters.
		 * 
		 */
		struct NativeAudioParams
		{
			/** @copydoc destructor */
			virtual ~NativeAudioParams() = default;
		};
	}
}