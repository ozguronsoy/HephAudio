#include "TriangleWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	TriangleWaveOscillator::TriangleWaveOscillator(uint32_t sampleRate) : Oscillator(0.5hf, 1500.0hf, sampleRate, 0hf, AngleUnit::Radian) {}
	TriangleWaveOscillator::TriangleWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase, AngleUnit angleUnit)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	heph_float TriangleWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		return this->peakAmplitude * (2.0hf / Math::pi) * asin(sin(2.0hf * Math::pi * this->frequency * t_sample / this->sampleRate + this->phase_rad));
	}
}