#pragma once
#include "HephAudioFramework.h"
#include "Oscillator.h"

namespace HephAudio
{
	class SquareWaveOscillator final : public Oscillator
	{
	public:
		SquareWaveOscillator();
		SquareWaveOscillator(uint32_t sampleRate);
		SquareWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad);
		heph_float operator[](size_t n) const override;
	};
}