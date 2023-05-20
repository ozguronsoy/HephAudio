#include "SineWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	SineWaveOscillator::SineWaveOscillator(uint32_t sampleRate) : Oscillator(0.5, 1500.0, sampleRate, 0.0, AngleUnit::Radian) {}
	SineWaveOscillator::SineWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase, AngleUnit angleUnit)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	heph_float SineWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		return this->peakAmplitude * sin(2.0 * Math::pi * this->frequency * t_sample / this->sampleRate + this->phase_rad);
	}
}