#include "Oscillators/TriangleWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	TriangleWaveOscillator::TriangleWaveOscillator() : Oscillator() {}
	TriangleWaveOscillator::TriangleWaveOscillator(uint32_t sampleRate) : Oscillator(0.5, 1500.0, sampleRate, 0) {}
	TriangleWaveOscillator::TriangleWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	heph_float TriangleWaveOscillator::operator[](size_t n) const
	{
		return this->peakAmplitude * (2.0 / Math::pi) * asin(sin(2.0 * Math::pi * this->frequency * n / this->sampleRate + this->phase_rad));
	}
}