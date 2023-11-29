#pragma once
#include "HephAudioFramework.h"
#include "Oscillator.h"

namespace HephAudio
{
	class SawToothWaveOscillator final : public Oscillator
	{
	public:
		SawToothWaveOscillator();
		SawToothWaveOscillator(uint32_t sampleRate);
		SawToothWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad);
		heph_float operator[](size_t n) const noexcept override;
	};
}