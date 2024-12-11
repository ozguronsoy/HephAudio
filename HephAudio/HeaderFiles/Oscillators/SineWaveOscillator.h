#pragma once
#include "HephAudioShared.h"
#include "Oscillator.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates sine wave signals.
	 * 
	 */
	class HEPH_API SineWaveOscillator final : public Oscillator
	{
	public:
		/** @copydoc default_constructor */
		SineWaveOscillator();

		/** 
		 * @copydoc constructor 
		 * 
		 * @param sampleRate @copydetails sampleRate
		 */
		explicit SineWaveOscillator(uint32_t sampleRate);

		/** @copydoc Oscillator(double,double,uint32_t,double) */
		SineWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);

		double operator[](size_t n) const override;
	};
}