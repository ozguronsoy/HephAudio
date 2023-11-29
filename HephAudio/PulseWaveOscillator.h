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
		PulseWaveOscillator();
		PulseWaveOscillator(uint32_t sampleRate);
		PulseWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad);
		heph_float operator[](size_t n) const noexcept override;
		heph_float GetPulseWidth() const noexcept;
		heph_float GetEta() const noexcept;
		void UpdateEta() noexcept;
	};
}