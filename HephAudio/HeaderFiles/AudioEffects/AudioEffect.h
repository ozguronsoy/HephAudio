#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include <string>

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for the audio effects.
	 *
	 */
	class AudioEffect
	{
	protected:
		/**
		 * number of threads that will be used, 1 by default.
		 */
		size_t threadCount;

	protected:
		/** @copydoc default_constructor */
		AudioEffect();

		/**
		 * @copydoc constructor
		 *
		 * @param threadCount @copydetails threadCount
		 */
		explicit AudioEffect(size_t threadCount);

	public:
		/** @copydoc destructor */
		virtual ~AudioEffect() = default;

		/**
		 * gets the name of the audio effect.
		 *
		 */
		virtual std::string Name() const = 0;

		/**
		 * gets the number of threads that will be used.
		 *
		 */
		virtual size_t GetThreadCount() const;

		/**
		 * sets the number of threads that will be used.
		 *
		 * @param threadCount @copydetails threadCount
		 */
		virtual void SetThreadCount(size_t threadCount);

		/**
		 * Applies the effect.
		 * @param buffer contains the audio data which will be processed.
		 */
		virtual void Process(AudioBuffer& buffer);

	protected:
		/**
		 * applies the effect using single thread.
		 * @param buffer contains the audio data which will be processed.
		 * @param startIndex index of the first sample to process.
		 * @param endIndex index of the last sample to process.
		 *
		 */
		virtual void ProcessST(AudioBuffer& buffer, size_t startIndex, size_t endIndex) = 0;

		/**
		 * applies the effect using multiple threads.
		 * @param buffer contains the audio data which will be processed.
		 *
		 */
		virtual void ProcessMT(AudioBuffer& buffer);
	};
}