#include "PulseWaveOscillator.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	PulseWaveOscillator::PulseWaveOscillator() : Oscillator() {}
	PulseWaveOscillator::PulseWaveOscillator(uint32_t sampleRate) : PulseWaveOscillator(0.5, 1500.0, sampleRate, 0) {}
	PulseWaveOscillator::PulseWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad)
		, dutyCycle(0.2), order(10)
	{
		this->UpdateEta();
	}
	heph_float PulseWaveOscillator::operator[](size_t n) const noexcept
	{
		const heph_float wt = 2.0 * Math::pi * this->frequency * n / this->sampleRate;
		const heph_float pid = Math::pi * this->dutyCycle;
		heph_float sample = 0.0;

		for (size_t k = 1; k < this->order + 1; k++)
		{
			sample += sin(pid * k) * cos(wt * k + this->phase_rad) / k;
		}

		sample *= 2.0 / Math::pi;
		sample += this->dutyCycle;
		sample *= this->peakAmplitude * this->eta;

		return sample;
	}
	heph_float PulseWaveOscillator::GetPulseWidth() const noexcept
	{
		return this->dutyCycle * this->frequency;
	}
	heph_float PulseWaveOscillator::GetEta() const noexcept
	{
		return this->eta;
	}
	void PulseWaveOscillator::UpdateEta() noexcept
	{
		const size_t frameCount = ceil(this->sampleRate / this->frequency);
		heph_float maxSample = this->peakAmplitude;
		this->eta = 1.0;

		for (size_t i = 0; i < frameCount; i++)
		{
			const heph_float currentSample = abs((*this)[i]);
			if (currentSample > maxSample)
			{
				maxSample = currentSample;
			}
		}

		this->eta = this->peakAmplitude / maxSample;
	}
}