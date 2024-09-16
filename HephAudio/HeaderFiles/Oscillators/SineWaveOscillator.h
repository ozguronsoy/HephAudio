#pragma once
#include "HephAudioShared.h"
#include "Oscillator.h"

/** @file */

namespace HephAudio
{
	class SineWaveOscillator final : public Oscillator
	{
	public:
		SineWaveOscillator();
		SineWaveOscillator(uint32_t sampleRate);
		SineWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);
		double operator[](size_t n) const override;
	};
}