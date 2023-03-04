#include "SineWaveOscillator.h"

namespace HephAudio
{
	SineWaveOscillator::SineWaveOscillator(const uint32_t& sampleRate) : OscillatorBase(0.5, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	SineWaveOscillator::SineWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	HEPHAUDIO_DOUBLE SineWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		return this->peakAmplitude * sin(this->w_sample * frameIndex + this->phase_rad);
	}
}