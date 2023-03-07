#include "PulseWaveOscillator.h"

namespace HephAudio
{
	PulseWaveOscillator::PulseWaveOscillator(const uint32_t& sampleRate) : PulseWaveOscillator(0.5, 1500.0, sampleRate, 0, AngleUnit::Radian) {}
	PulseWaveOscillator::PulseWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit)
	{
		this->SetDutyCycle(0.2);
		this->SetOrder(10);
		this->UpdateEta();
	}
	hephaudio_float PulseWaveOscillator::Oscillate(const size_t& frameIndex) const noexcept
	{
		const hephaudio_float wt = this->w_sample * frameIndex;
		const hephaudio_float pid = PI * this->dutyCycle;
		hephaudio_float sample = 0.0;

		for (size_t n = 1; n < this->order + 1; n++)
		{
			sample += sin(pid * n) * cos(wt * n + this->phase_rad) / n;
		}

		sample *= 2.0 / PI;
		sample += this->dutyCycle;
		sample *= this->peakAmplitude * this->eta;

		return sample;
	}
	void PulseWaveOscillator::SetPeakAmplitude(hephaudio_float peakAmplitude) noexcept
	{
		OscillatorBase::SetPeakAmplitude(peakAmplitude);
	}
	const hephaudio_float& PulseWaveOscillator::GetDutyCycle() const noexcept
	{
		return this->dutyCycle;
	}
	void PulseWaveOscillator::SetDutyCycle(hephaudio_float dutyCycle) noexcept
	{
		this->dutyCycle = dutyCycle;
	}
	const hephaudio_float& PulseWaveOscillator::GetPulseWidth() const noexcept
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
	void PulseWaveOscillator::UpdateEta() noexcept
	{
		const size_t frameCount = ceil(this->sampleRate / this->frequency);
		hephaudio_float maxSample = 1.0;
		this->eta = 1.0;

		for (size_t i = 0; i < frameCount; i++)
		{
			const hephaudio_float currentSample = abs(this->Oscillate(i));
			if (currentSample > maxSample)
			{
				maxSample = currentSample;
			}
		}

		this->eta = 1.0 / maxSample;
	}
}