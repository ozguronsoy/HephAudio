#include "TriangleWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	TriangleWaveOscillator::TriangleWaveOscillator(uint32_t sampleRate) : Oscillator(0.5, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	TriangleWaveOscillator::TriangleWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase, AngleUnit angleUnit)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	heph_float TriangleWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		return this->peakAmplitude * (2.0 / Math::pi) * asin(sin(2.0 * Math::pi * this->frequency * t_sample / this->sampleRate + this->phase_rad));
	}
}