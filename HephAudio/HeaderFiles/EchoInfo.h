#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include <cstdint>

/** @file */

namespace HephAudio
{
	/**
	 * @brief container for echo effect parameters.
	 * 
	 */
	struct EchoInfo
	{
		/**
		 * number of times the audio data will be reflected (repeated).
		 * 
		 */
		uint32_t reflectionCount;

		/**
		 * elapsed time, in seconds, between the start of each reflection.
		 * 
		 */
		double reflectionDelay_s;

		/**
		 * multiplication factor of the reflected data. 
		 * Reflected data will be multiplied by this between each reflection.
		 * 
		 */
		double decayFactor;

		/**
		 * start position of the audio data that will be reflected. 
		 * Should be between 0 and 1.
		 * 
		 */
		double echoStartPosition;

		/**
		 * end position of the audio data that will be reflected. 
		 * Should be between 0 and 1.
		 * 
		 * @note the \a echoStartPosition must be less than or equal to \a echoEndPosition.
		 */
		double echoEndPosition;
		
		EchoInfo();
		EchoInfo(uint32_t reflectionCount, double reflectionDelay_s, double decayFactor, double echoStartPosition, double echoEndPosition);

		/**
		 * calculates the number of frames the buffer will have after applying the echo effect.
		 * 
		 */
		size_t CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const;
	};
}