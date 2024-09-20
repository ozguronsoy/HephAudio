#pragma once
#include "HephAudioShared.h"
#include "EventResult.h"
#include "AudioBuffer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief struct for storing the results of the audio render events.
	 * 
	 */
	struct HEPH_API AudioRenderEventResult : public HephCommon::EventResult
	{
		/**
		 * contains the audio data that will be rendered.
		 * 
		 */
		AudioBuffer renderBuffer;

		/**
		 * indicates whether this is the last audio data to be rendered.
		 * 
		 */
		bool isFinishedPlaying;

		/** @copydoc default_constructor */
		AudioRenderEventResult();
	};
}