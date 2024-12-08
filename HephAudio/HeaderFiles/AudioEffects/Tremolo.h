#pragma once
#include "ModulationEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief changes the volume of the audio data periodically and mixes the result with the input signal.
	 */
	class HEPH_API Tremolo : public ModulationEffect
	{
	public:
		using ModulationEffect::Process;

	public:
		/** @copydoc default_constructor */
		Tremolo();

		/** @copydoc ModulationEffect(double, const Oscillator&) */
		Tremolo(double depth, const Oscillator& lfo);
		
		/** @copydoc destructor */
		virtual ~Tremolo() = default;

		virtual std::string Name() const override;
		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}