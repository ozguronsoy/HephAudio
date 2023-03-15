#include "SquareWaveOscillator.h"

namespace HephAudio
{
	SquareWaveOscillator::SquareWaveOscillator(uint32_t sampleRate) : Oscillator(0.05, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	SquareWaveOscillator::SquareWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase, AngleUnit angleUnit)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	hephaudio_float SquareWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		return this->peakAmplitude * sgn(sin(2.0 * PI * this->frequency * t_sample / this->sampleRate + this->phase_rad));
	}
}