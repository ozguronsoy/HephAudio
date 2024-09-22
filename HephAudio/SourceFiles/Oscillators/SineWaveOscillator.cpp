#include "Oscillators/SineWaveOscillator.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	SineWaveOscillator::SineWaveOscillator() : Oscillator() {}
	SineWaveOscillator::SineWaveOscillator(uint32_t sampleRate) : Oscillator(0.5, 1500.0, sampleRate, 0) {}
	SineWaveOscillator::SineWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	double SineWaveOscillator::operator[](size_t n) const
	{
		return this->peakAmplitude * sin(2.0 * HEPH_MATH_PI * this->frequency * n / this->sampleRate + this->phase_rad);
	}
}