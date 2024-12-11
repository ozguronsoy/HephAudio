#pragma once
#include "HephAudioShared.h"
#include "Oscillator.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates sawtooth wave signals.
	 * 
	 */
	class HEPH_API SawToothWaveOscillator final : public Oscillator
	{
	public:
		/** @copydoc default_constructor */
		SawToothWaveOscillator();

		/** 
		 * @copydoc constructor 
		 * 
		 * @param sampleRate @copydetails sampleRate
		 */
		explicit SawToothWaveOscillator(uint32_t sampleRate);

		/** @copydoc Oscillator(double,double,uint32_t,double) */
		SawToothWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);
		
		double operator[](size_t n) const override;
	};
}