#pragma once
#include "HephAudioShared.h"
#include "Oscillator.h"

namespace HephAudio
{
	class SquareWaveOscillator final : public Oscillator
	{
	public:
		SquareWaveOscillator();
		SquareWaveOscillator(uint32_t sampleRate);
		SquareWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);
		double operator[](size_t n) const override;
	};
}