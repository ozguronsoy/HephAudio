#pragma once
#include "HephAudioShared.h"
#include "Oscillator.h"

/** @file */

namespace HephAudio
{
	class SawToothWaveOscillator final : public Oscillator
	{
	public:
		SawToothWaveOscillator();
		SawToothWaveOscillator(uint32_t sampleRate);
		SawToothWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);
		double operator[](size_t n) const override;
	};
}