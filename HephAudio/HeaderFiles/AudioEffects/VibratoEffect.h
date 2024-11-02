#pragma once
#include "HephAudioShared.h"
#include "ModulationEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief changes the pitch of the audio data periodically and mixes the result with the input signal.
	 * 
	 * @note this is a non-causal effect.
	 */
	class VibratoEffect final : public ModulationEffect
	{
	public:
		/**
		 * maximum pitch change in terms of semitones.
		 */
		double extent;

	public:
		/** @copydoc default_constructor */
		VibratoEffect();

		/**
		 * @copydoc ModulationEffect(double, Oscillator&) 
		 * 
		 * @param extent @copydetails extent
		 */
		VibratoEffect(double depth, double extent, Oscillator& lfo);

		std::string Name() const override;

		size_t CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const override;

	private:
		void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}