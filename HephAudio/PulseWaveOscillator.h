#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class PulseWaveOscillator : public OscillatorBase
	{
	protected:
		hephaudio_float eta;
	public:
		uint32_t order;
		hephaudio_float dutyCycle;
	public:
		PulseWaveOscillator(uint32_t sampleRate);
		PulseWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase = 0.0, AngleUnit angleUnit = AngleUnit::Radian);
		virtual ~PulseWaveOscillator() = default;
		virtual hephaudio_float Oscillate(size_t t_sample) const noexcept override;
		virtual hephaudio_float GetPulseWidth() const noexcept;
		virtual hephaudio_float GetEta() const noexcept;
		virtual void UpdateEta() noexcept;
	};
}