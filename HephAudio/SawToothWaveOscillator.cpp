#include "SawToothWaveOscillator.h"

namespace HephAudio
{
	SawToothWaveOscillator::SawToothWaveOscillator(const uint32_t& sampleRate) : OscillatorBase(0.05, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	SawToothWaveOscillator::SawToothWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	HEPHAUDIO_DOUBLE SawToothWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		const HEPHAUDIO_DOUBLE t = (HEPHAUDIO_DOUBLE)frameIndex / this->sampleRate + RadToDeg(this->phase_rad);
		const HEPHAUDIO_DOUBLE ft = this->frequency * t;
		return this->peakAmplitude * 2.0 * (ft - round(ft));
	}
}