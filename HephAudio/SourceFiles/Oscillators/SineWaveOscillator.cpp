#include "Oscillators/SineWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	SineWaveOscillator::SineWaveOscillator() : Oscillator() {}
	SineWaveOscillator::SineWaveOscillator(uint32_t sampleRate) : Oscillator(0.5, 1500.0, sampleRate, 0) {}
	SineWaveOscillator::SineWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	heph_float SineWaveOscillator::operator[](size_t n) const
	{
		return this->peakAmplitude * sin(2.0 * HEPH_MATH_PI * this->frequency * n / this->sampleRate + this->phase_rad);
	}
}