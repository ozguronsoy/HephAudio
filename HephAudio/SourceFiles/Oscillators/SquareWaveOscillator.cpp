#include "Oscillators/SquareWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	SquareWaveOscillator::SquareWaveOscillator() : Oscillator() {}
	SquareWaveOscillator::SquareWaveOscillator(uint32_t sampleRate) : Oscillator(0.05, 1500.0, sampleRate, 0) {}
	SquareWaveOscillator::SquareWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	heph_float SquareWaveOscillator::operator[](size_t n) const
	{
		const heph_float sample = sin(2.0 * HEPH_MATH_PI * this->frequency * n / this->sampleRate + this->phase_rad);
		return this->peakAmplitude * HEPH_MATH_SGN(sample);
	}
}