#pragma once
#include "HephAudioShared.h"
#include "Oscillator.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates square wave signals.
	 * 
	 */
	class HEPH_API SquareWaveOscillator final : public Oscillator
	{
	public:
		/** @copydoc default_constructor */
		SquareWaveOscillator();

		/** 
		 * @copydoc constructor 
		 * 
		 * @param sampleRate @copydetails sampleRate
		 */
		explicit SquareWaveOscillator(uint32_t sampleRate);

		/** @copydoc Oscillator(double,double,uint32_t,double) */
		SquareWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);

		double operator[](size_t n) const override;
	};
}