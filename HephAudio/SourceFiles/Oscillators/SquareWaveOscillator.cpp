#include "Oscillators/SquareWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	SquareWaveOscillator::SquareWaveOscillator() : Oscillator() {}
	SquareWaveOscillator::SquareWaveOscillator(uint32_t sampleRate) : Oscillator(0.05, 1500.0, sampleRate, 0) {}
	SquareWaveOscillator::SquareWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	double SquareWaveOscillator::operator[](size_t n) const
	{
		const double sample = sin(2.0 * HEPH_MATH_PI * this->frequency * n / this->sampleRate + this->phase_rad);
		return this->peakAmplitude * HEPH_MATH_SGN(sample);
	}
}