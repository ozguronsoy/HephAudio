#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class SineWaveOscillator : public OscillatorBase
	{
	public:
		SineWaveOscillator(const uint32_t& sampleRate);
		SineWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit = AngleUnit::Radian);
		virtual ~SineWaveOscillator() = default;
		virtual HEPHAUDIO_DOUBLE Oscillate(const size_t& frameIndex) const noexcept override;
	};
}