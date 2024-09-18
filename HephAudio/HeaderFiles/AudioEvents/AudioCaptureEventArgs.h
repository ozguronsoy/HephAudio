#pragma once
#include "HephAudioShared.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief struct for storing the arguments for the audio capture events.
	 * 
	 */
	struct AudioCaptureEventArgs : public AudioEventArgs
	{
		/**
		 * captured audio data.
		 * 
		 */
		AudioBuffer captureBuffer;

		/**
		 * creates a new instance and initializes it with the provided values.
		 * 
		 */
		AudioCaptureEventArgs(void* pNativeAudio, AudioBuffer& captureBuffer) : AudioEventArgs(pNativeAudio), captureBuffer(std::move(captureBuffer)) {}
	};
}