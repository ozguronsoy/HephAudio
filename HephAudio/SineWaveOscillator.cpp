#include "SineWaveOscillator.h"

namespace HephAudio
{
	SineWaveOscillator::SineWaveOscillator(uint32_t sampleRate) : Oscillator(0.5, 1500.0, sampleRate, 0.0, AngleUnit::Radian) {}
	SineWaveOscillator::SineWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase, AngleUnit angleUnit)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	hephaudio_float SineWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		return this->peakAmplitude * sin(2.0 * PI * this->frequency * t_sample / this->sampleRate + this->phase_rad);
	}
}