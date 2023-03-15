#include "SawToothWaveOscillator.h"

namespace HephAudio
{
	SawToothWaveOscillator::SawToothWaveOscillator(uint32_t sampleRate) : Oscillator(0.05, 1500.0, sampleRate, 0.0, AngleUnit::Radian) {}
	SawToothWaveOscillator::SawToothWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase, AngleUnit angleUnit)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	hephaudio_float SawToothWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		const hephaudio_float t_second = (hephaudio_float)t_sample / this->sampleRate + RadToDeg(this->phase_rad);
		const hephaudio_float ft = this->frequency * t_second;
		return this->peakAmplitude * 2.0 * (this->frequency * t_second - round(this->frequency * t_second));
	}
}