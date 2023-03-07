#include "SawToothWaveOscillator.h"

namespace HephAudio
{
	SawToothWaveOscillator::SawToothWaveOscillator(const uint32_t& sampleRate) : OscillatorBase(0.05, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	SawToothWaveOscillator::SawToothWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	hephaudio_float SawToothWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		const hephaudio_float t = (hephaudio_float)frameIndex / this->sampleRate + RadToDeg(this->phase_rad);
		const hephaudio_float ft = this->frequency * t;
		return this->peakAmplitude * 2.0 * (ft - round(ft));
	}
}