#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for audio effects that use a temporary buffer while processing.
	 */
	class DoubleBufferedAudioEffect : public AudioEffect
	{
	public:
		using AudioEffect::Process;

	protected:
		/** @copydoc default_constructor */
		DoubleBufferedAudioEffect();

	public:
		/** @copydoc destructor */
		virtual ~DoubleBufferedAudioEffect() = default;

		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

	protected:
		/**
		 * creates the output buffer but does not initialize it.
		 * 
		 * @param inputBuffer contains the audio data which will be processed.
		 * @param startIndex index of the first sample to process.
		 * @param frameCount number of frames to process.
		 *
		 */
		virtual AudioBuffer CreateOutputBuffer(const AudioBuffer& inputBuffer, size_t startIndex, size_t frameCount) const;

		/**
		 * initializes the output buffer.
		 * 
		 * @param inputBuffer contains the audio data which will be processed.
		 * @param outputBuffer contains the processed audio data.
		 * @param startIndex index of the first sample to process.
		 * @param frameCount number of frames to process.
		 * 
		 */
		virtual void InitializeOutputBuffer(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) const;
	};
}