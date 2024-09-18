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
		 * @copydoc constructor
		 * 
		 * @param pNativeAudio @copydetails AudioEventArgs::pNativeAudio
		 * @param captureBuffer @copydetails captureBuffer
		 */
		AudioCaptureEventArgs(Native::NativeAudio* pNativeAudio, AudioBuffer& captureBuffer) : AudioEventArgs(pNativeAudio), captureBuffer(std::move(captureBuffer)) {}
	};
}