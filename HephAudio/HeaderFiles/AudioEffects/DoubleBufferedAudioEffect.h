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

		/** @copydoc AudioEffect(size_t) */
		explicit DoubleBufferedAudioEffect(size_t threadCount);

	public:
		/** @copydoc destructor */
		virtual ~DoubleBufferedAudioEffect() = default;

		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;
	};
}