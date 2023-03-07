#include "TriangleWaveOscillator.h"

namespace HephAudio
{
	TriangleWaveOscillator::TriangleWaveOscillator(const uint32_t& sampleRate) : OscillatorBase(0.5, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	TriangleWaveOscillator::TriangleWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	hephaudio_float TriangleWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		return this->peakAmplitude * (2.0 / PI) * asin(sin(this->w_sample * frameIndex));
	}
}