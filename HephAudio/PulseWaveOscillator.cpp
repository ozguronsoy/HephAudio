#include "PulseWaveOscillator.h"

namespace HephAudio
{
	PulseWaveOscillator::PulseWaveOscillator(uint32_t sampleRate) : PulseWaveOscillator(0.5, 1500.0, sampleRate) {}
	PulseWaveOscillator::PulseWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase, AngleUnit angleUnit)
		: OscillatorBase(peakAmplitude, frequency, sampleRate, phase, angleUnit)
		, dutyCycle(0.2), order(10)
	{
		this->UpdateEta();
	}
	hephaudio_float PulseWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		const hephaudio_float wt = 2.0 * PI * this->frequency * t_sample / this->sampleRate;
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
	hephaudio_float PulseWaveOscillator::GetPulseWidth() const noexcept
	{
		return this->dutyCycle * this->frequency;
	}
	hephaudio_float PulseWaveOscillator::GetEta() const noexcept
	{
		return this->eta;
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