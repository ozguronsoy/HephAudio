#include "TriangleWaveOscillator.h"

namespace HephAudio
{
	TriangleWaveOscillator::TriangleWaveOscillator(uint32_t sampleRate) : OscillatorBase(0.5, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	TriangleWaveOscillator::TriangleWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase, AngleUnit angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	hephaudio_float TriangleWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		return this->peakAmplitude * (2.0 / PI) * asin(sin(2.0 * PI * this->frequency * t_sample / this->sampleRate + this->phase_rad));
	}
}