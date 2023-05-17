#pragma once
#include "HephAudioFramework.h"
#include "Oscillator.h"

namespace HephAudio
{
	class PulseWaveOscillator final : public Oscillator
	{
	private:
		heph_float eta;
	public:
		uint32_t order;
		heph_float dutyCycle;
	public:
		PulseWaveOscillator(uint32_t sampleRate);
		PulseWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase = 0.0, AngleUnit angleUnit = AngleUnit::Radian);
		heph_float Oscillate(size_t t_sample) const noexcept override;
		heph_float GetPulseWidth() const noexcept;
		heph_float GetEta() const noexcept;
		void UpdateEta() noexcept;
	};
}