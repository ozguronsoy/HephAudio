#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class PulseWaveOscillator : public OscillatorBase
	{
	protected:
		HEPHAUDIO_DOUBLE dutyCycle;
		uint32_t order;
	public:
		PulseWaveOscillator(const uint32_t& sampleRate);
		PulseWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit = AngleUnit::Radian);
		virtual ~PulseWaveOscillator() = default;
		virtual HEPHAUDIO_DOUBLE Oscillate(const size_t& frameIndex) const noexcept override;
		const HEPHAUDIO_DOUBLE& GetDutyCycle() const noexcept;
		void SetDutyCycle(HEPHAUDIO_DOUBLE dutyCycle) noexcept;
		const HEPHAUDIO_DOUBLE& GetPulseWidth() const noexcept;
		const uint32_t& GetOrder() const noexcept;
		void SetOrder(uint32_t order) noexcept;
	};
}