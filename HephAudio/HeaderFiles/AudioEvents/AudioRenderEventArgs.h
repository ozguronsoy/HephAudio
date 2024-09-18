#pragma once
#include "HephAudioShared.h"
#include "AudioEventArgs.h"
#include "AudioBuffer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief struct for storing the arguments for the audio render events.
	 * 
	 */
	struct AudioRenderEventArgs : public AudioEventArgs
	{
		/**
		 * pointer to the audio object instance that's being rendered.
		 * 
		 */
		void* pAudioObject;

		/**
		 * number of audio frames to  render.
		 * 
		 */
		size_t renderFrameCount;

		/**
		 * creates a new instance and initializes it with the provided values.
		 * 
		 */
		AudioRenderEventArgs(void* pAudioObject, void* pNativeAudio, size_t renderFrameCount) 
			: AudioEventArgs(pNativeAudio), pAudioObject(pAudioObject), renderFrameCount(renderFrameCount) {}
	};
}