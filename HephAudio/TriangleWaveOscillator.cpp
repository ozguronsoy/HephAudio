#include "TriangleWaveOscillator.h"

namespace HephAudio
{
	TriangleWaveOscillator::TriangleWaveOscillator(const uint32_t& sampleRate) : OscillatorBase(0.5, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	TriangleWaveOscillator::TriangleWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	HEPHAUDIO_DOUBLE TriangleWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		return this->peakAmplitude * (2.0 / PI) * asin(sin(this->w_sample * frameIndex));
	}
}