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
	class HEPH_API AudioEffect
	{
	protected:
		/**
		 * number of threads that will be used.
		 */
		size_t threadCount;

	protected:
		/** @copydoc default_constructor */
		AudioEffect();

	public:
		/** @copydoc destructor */
		virtual ~AudioEffect() = default;

		/**
		 * gets the name of the audio effect.
		 *
		 */
		virtual std::string Name() const = 0;

		/**
		 * checks whether the effect can be applied using multiple threads.
		 * 
		 */
		virtual bool HasMTSupport() const;

		/**
		 * checks whether the effect can be applied in real-time.
		 * 
		 */
		virtual bool HasRTSupport() const;

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
		 * calculates the number of frames required to obtain the number of frames desired for the output buffer.
		 *
		 * @param outputFrameCount the number of frames desired for the output buffer.
		 * @param formatInfo the format info of the input buffer.
		 *
		 */
		virtual size_t CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const;

		/**
		 * calculates the number of frames the buffer will contain after applying the effect.
		 * 
		 * @param inputFrameCount the number of frames of the input buffer.
		 * @param formatInfo the format info of the input buffer.
		 * 
		 */
		virtual size_t CalculateOutputFrameCount(size_t inputFrameCount, const AudioFormatInfo& formatInfo) const;

		/**
		 * calculates the number of frames to process and advance for the current render event.
		 * 
		 * @param renderFrameCount number of audio frames that will be rendered.
		 * @param formatInfo the format info of the input buffer.
		 * 
		 */
		virtual size_t CalculateAdvanceSize(size_t renderFrameCount, const AudioFormatInfo& formatInfo) const;

		/**
		 * resets the variables internally used for processing.
		 * 
		 */
		virtual void ResetInternalState();

		/**
		 * Applies the effect.
		 * 
		 * @param buffer contains the audio data which will be processed.
		 * 
		 */
		virtual void Process(AudioBuffer& buffer);

		/**
		 * Applies the effect.
		 * 
		 * @param buffer contains the audio data which will be processed.
		 * @param startIndex index of the first audio frame to process.
		 * 
		 */
		virtual void Process(AudioBuffer& buffer, size_t startIndex);

		/**
		 * Applies the effect.
		 * 
		 * @param buffer contains the audio data which will be processed.
		 * @param startIndex index of the first audio frame to process.
		 * @param frameCount number of frames to process.
		 * 
		 */
		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount);

	protected:
		/**
		 * applies the effect using single thread.
		 * 
		 * @param inputBuffer contains the audio data which will be processed.
		 * @param outputBuffer contains the processed audio data.
		 * @param startIndex index of the first sample to process.
		 * @param frameCount number of frames to process.
		 *
		 */
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) = 0;

		/**
		 * applies the effect using multiple threads.
		 * 
		 * @param inputBuffer contains the audio data which will be processed.
		 * @param outputBuffer contains the processed audio data.
		 * @param startIndex index of the first sample to process.
		 * @param frameCount number of frames to process.
		 *
		 */
		virtual void ProcessMT(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount);
	};
}