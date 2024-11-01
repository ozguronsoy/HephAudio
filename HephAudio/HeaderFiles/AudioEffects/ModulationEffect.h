#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"
#include "Buffers/DoubleBuffer.h"
#include "Oscillators/Oscillator.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for effects that use LFOs.
	 */
	class ModulationEffect : public AudioEffect
	{
	public:
		using AudioEffect::Process;

	protected:
		/**
		 * contains one period long data.
		 */
		Heph::DoubleBuffer lfoBuffer;

		/**
		 * index of the last accessed sample.
		 */
		size_t lfoIndex;

		/**
		 * controls the dry/wet mix ratio.
		 * Ranges between 0 and 1.
		 *
		 */
		double depth;

	protected:
		/** @copydoc default_constructor */
		ModulationEffect();

		/**
		 * @copydoc constructor
		 * @param depth @copydetails depth
		 * @param lfo low-frequency oscillator.
		 * 
		 */
		ModulationEffect(double depth, Oscillator& lfo);
		
	public:
		/** @copydoc destructor */
		virtual ~ModulationEffect() = default;

		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

		/**
		 * sets the lfo.
		 * @param lfo low-frequency oscillator.
		 */
		virtual void SetOscillator(Oscillator& lfo);

		/**
		 * gets the depth.
		 *
		 */
		virtual double GetDepth() const;

		/**
		 * sets the depth.
		 * @param depth @copydetails depth
		 */
		virtual void SetDepth(double depth);
	};
}