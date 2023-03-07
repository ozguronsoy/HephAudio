#include "SquareWaveOscillator.h"

namespace HephAudio
{
	SquareWaveOscillator::SquareWaveOscillator(const uint32_t& sampleRate) : OscillatorBase(0.05, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	SquareWaveOscillator::SquareWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	hephaudio_float SquareWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		return this->peakAmplitude * sgn(sin(this->w_sample * frameIndex + this->phase_rad));
	}
}