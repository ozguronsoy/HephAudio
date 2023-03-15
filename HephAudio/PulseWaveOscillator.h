#pragma once
#include "framework.h"
#include "Oscillator.h"

namespace HephAudio
{
	class PulseWaveOscillator final : public Oscillator
	{
	private:
		hephaudio_float eta;
	public:
		uint32_t order;
		hephaudio_float dutyCycle;
	public:
		PulseWaveOscillator(uint32_t sampleRate);
		PulseWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase = 0.0, AngleUnit angleUnit = AngleUnit::Radian);
		hephaudio_float Oscillate(size_t t_sample) const noexcept override;
		hephaudio_float GetPulseWidth() const noexcept;
		hephaudio_float GetEta() const noexcept;
		void UpdateEta() noexcept;
	};
}