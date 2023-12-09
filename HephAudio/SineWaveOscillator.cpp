#include "SineWaveOscillator.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	SineWaveOscillator::SineWaveOscillator() : Oscillator() {}
	SineWaveOscillator::SineWaveOscillator(uint32_t sampleRate) : Oscillator(0.5, 1500.0, sampleRate, 0) {}
	SineWaveOscillator::SineWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	heph_float SineWaveOscillator::operator[](size_t n) const noexcept
	{
		return this->peakAmplitude * sin(2.0 * Math::pi * this->frequency * n / this->sampleRate + this->phase_rad);
	}
}