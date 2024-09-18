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
			/**
			 * releases the resources and destroys the instance.
			 * 
			 */
			virtual ~NativeAudioParams() = default;
		};
	}
}