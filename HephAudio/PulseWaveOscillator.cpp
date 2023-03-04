#include "PulseWaveOscillator.h"

namespace HephAudio
{
	PulseWaveOscillator::PulseWaveOscillator(const uint32_t& sampleRate) : PulseWaveOscillator(0.5, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	PulseWaveOscillator::PulseWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit)
	{
		this->SetDutyCycle(0.2);
		this->SetOrder(10);
	}
	HEPHAUDIO_DOUBLE PulseWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		const HEPHAUDIO_DOUBLE wt = this->w_sample * frameIndex;
		const HEPHAUDIO_DOUBLE pid = PI * this->dutyCycle;
		HEPHAUDIO_DOUBLE sample = 0.0;

		for (size_t n = 1; n < this->order + 1; n++)
		{
			sample += sin(pid * n) * cos(wt * n + this->phase_rad) / n;
		}

		sample *= 2.0 * this->peakAmplitude / PI;
		sample += this->peakAmplitude * this->dutyCycle - 0.5;
		sample *= 2.0;

		return sample;
	}
	const HEPHAUDIO_DOUBLE& PulseWaveOscillator::GetDutyCycle() const noexcept
	{
		return this->dutyCycle;
	}
	void PulseWaveOscillator::SetDutyCycle(HEPHAUDIO_DOUBLE dutyCycle) noexcept
	{
		if (dutyCycle >= 0.0 && dutyCycle <= 1.0)
		{
			this->dutyCycle = dutyCycle;
		}
	}
	const HEPHAUDIO_DOUBLE& PulseWaveOscillator::GetPulseWidth() const noexcept
	{
		return this->dutyCycle * this->frequency;
	}
	const uint32_t& PulseWaveOscillator::GetOrder() const noexcept
	{
		return this->order;
	}
	void PulseWaveOscillator::SetOrder(uint32_t order) noexcept
	{
		this->order = order;
	}
}