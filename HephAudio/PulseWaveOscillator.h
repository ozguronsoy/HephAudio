#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class PulseWaveOscillator : public OscillatorBase
	{
	protected:
		uint32_t order;
		HEPHAUDIO_DOUBLE dutyCycle;
		HEPHAUDIO_DOUBLE eta;
	public:
		PulseWaveOscillator(const uint32_t& sampleRate);
		PulseWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit = AngleUnit::Radian);
		virtual ~PulseWaveOscillator() = default;
		virtual HEPHAUDIO_DOUBLE Oscillate(const size_t& frameIndex) const noexcept override;
		virtual void SetPeakAmplitude(HEPHAUDIO_DOUBLE peakAmplitude) noexcept override;
		virtual const HEPHAUDIO_DOUBLE& GetDutyCycle() const noexcept;
		virtual void SetDutyCycle(HEPHAUDIO_DOUBLE dutyCycle) noexcept;
		virtual const HEPHAUDIO_DOUBLE& GetPulseWidth() const noexcept;
		virtual const uint32_t& GetOrder() const noexcept;
		virtual void SetOrder(uint32_t order) noexcept;
		virtual void UpdateEta() noexcept;
	};
}