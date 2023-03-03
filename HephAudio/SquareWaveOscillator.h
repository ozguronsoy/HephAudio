#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class SquareWaveOscillator : public OscillatorBase
	{
	public:
		SquareWaveOscillator(const uint32_t& sampleRate);
		SquareWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit);
		virtual ~SquareWaveOscillator() = default;
		virtual HEPHAUDIO_DOUBLE Oscillate(const size_t& frameIndex) const noexcept override;
	};
}