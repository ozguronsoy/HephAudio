#include "SquareWaveOscillator.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	SquareWaveOscillator::SquareWaveOscillator() : Oscillator() {}
	SquareWaveOscillator::SquareWaveOscillator(uint32_t sampleRate) : Oscillator(0.05, 1500.0, sampleRate, 0) {}
	SquareWaveOscillator::SquareWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	heph_float SquareWaveOscillator::operator[](size_t n) const noexcept
	{
		return this->peakAmplitude * Math::Sgn(sin(2.0 * Math::pi * this->frequency * n / this->sampleRate + this->phase_rad));
	}
}