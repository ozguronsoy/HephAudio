#include "SineWaveOscillator.h"

namespace HephAudio
{
	SineWaveOscillator::SineWaveOscillator(const uint32_t& sampleRate) : OscillatorBase(0.5, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	SineWaveOscillator::SineWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	hephaudio_float SineWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		return this->peakAmplitude * sin(this->w_sample * frameIndex + this->phase_rad);
	}
}