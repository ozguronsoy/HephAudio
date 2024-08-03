#include "Oscillators/TriangleWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	TriangleWaveOscillator::TriangleWaveOscillator() : Oscillator() {}
	TriangleWaveOscillator::TriangleWaveOscillator(uint32_t sampleRate) : Oscillator(0.5, 1500.0, sampleRate, 0) {}
	TriangleWaveOscillator::TriangleWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	double TriangleWaveOscillator::operator[](size_t n) const
	{
		return this->peakAmplitude * (2.0 / HEPH_MATH_PI) * asin(sin(2.0 * HEPH_MATH_PI * this->frequency * n / this->sampleRate + this->phase_rad));
	}
}