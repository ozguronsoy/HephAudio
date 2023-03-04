#include "SquareWaveOscillator.h"

namespace HephAudio
{
	SquareWaveOscillator::SquareWaveOscillator(const uint32_t& sampleRate) : OscillatorBase(0.05, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	SquareWaveOscillator::SquareWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	HEPHAUDIO_DOUBLE SquareWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		return this->peakAmplitude * sgn(sin(this->w_sample * frameIndex + this->phase_rad));
	}
}