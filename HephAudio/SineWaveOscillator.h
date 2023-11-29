#pragma once
#include "HephAudioFramework.h"
#include "Oscillator.h"

namespace HephAudio
{
	class SineWaveOscillator final : public Oscillator
	{
	public:
		SineWaveOscillator();
		SineWaveOscillator(uint32_t sampleRate);
		SineWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad);
		heph_float operator[](size_t n) const noexcept override;
	};
}