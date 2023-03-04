#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class TriangleWaveOscillator : public OscillatorBase
	{
	public:
		TriangleWaveOscillator(const uint32_t& sampleRate);
		TriangleWaveOscillator(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit = AngleUnit::Radian);
		virtual ~TriangleWaveOscillator() = default;
		virtual HEPHAUDIO_DOUBLE Oscillate(const size_t& frameIndex) const noexcept override;
	};
}