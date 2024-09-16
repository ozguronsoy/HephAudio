#pragma once
#include "HephAudioShared.h"
#include "Oscillator.h"

/** @file */

namespace HephAudio
{
	class TriangleWaveOscillator final : public Oscillator
	{
	public:
		TriangleWaveOscillator();
		TriangleWaveOscillator(uint32_t sampleRate);
		TriangleWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);
		double operator[](size_t n) const override;
	};
}