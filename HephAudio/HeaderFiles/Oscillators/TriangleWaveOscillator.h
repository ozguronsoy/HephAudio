#pragma once
#include "HephAudioShared.h"
#include "Oscillator.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief generates triangle wave signals.
	 * 
	 */
	class HEPH_API TriangleWaveOscillator final : public Oscillator
	{
	public:
		/** @copydoc default_constructor */
		TriangleWaveOscillator();

		/** 
		 * @copydoc constructor 
		 * 
		 * @param sampleRate @copydetails sampleRate
		 */
		explicit TriangleWaveOscillator(uint32_t sampleRate);

		/** @copydoc Oscillator(double,double,uint32_t,double) */
		TriangleWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);

		double operator[](size_t n) const override;
	};
}