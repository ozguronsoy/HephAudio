#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class PulseWaveOscillator : public OscillatorBase
	{
	protected:
		uint32_t order;
		hephaudio_float dutyCycle;
		hephaudio_float eta;
	public:
		PulseWaveOscillator(const uint32_t& sampleRate);
		PulseWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit = AngleUnit::Radian);
		virtual ~PulseWaveOscillator() = default;
		virtual hephaudio_float Oscillate(const size_t& frameIndex) const noexcept override;
		virtual void SetPeakAmplitude(hephaudio_float peakAmplitude) noexcept override;
		virtual const hephaudio_float& GetDutyCycle() const noexcept;
		virtual void SetDutyCycle(hephaudio_float dutyCycle) noexcept;
		virtual const hephaudio_float& GetPulseWidth() const noexcept;
		virtual const uint32_t& GetOrder() const noexcept;
		virtual void SetOrder(uint32_t order) noexcept;
		virtual void UpdateEta() noexcept;
	};
}