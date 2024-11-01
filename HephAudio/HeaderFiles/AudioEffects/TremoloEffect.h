#pragma once
#include "ModulationEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief changes the volume of the audio data periodically and mixes the result with the input signal.
	 */
	class TremoloEffect final : public ModulationEffect
	{
	public:
		/** @copydoc default_constructor */
		TremoloEffect();

		/** @copydoc ModulationEffect(double, Oscillator&) */
		TremoloEffect(double depth, Oscillator& lfo);

		std::string Name() const override;

	private:
		void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t endIndex) override;
	};
}