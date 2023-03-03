#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class SawToothWaveOscillator : public OscillatorBase
	{
	public:
		SawToothWaveOscillator(const uint32_t& sampleRate);
		SawToothWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit);
		virtual ~SawToothWaveOscillator() = default;
		virtual HEPHAUDIO_DOUBLE Oscillate(const size_t& frameIndex) const noexcept override;
	};
}