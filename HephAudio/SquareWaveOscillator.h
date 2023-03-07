#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class SquareWaveOscillator : public OscillatorBase
	{
	public:
		SquareWaveOscillator(const uint32_t& sampleRate);
		SquareWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit = AngleUnit::Radian);
		virtual ~SquareWaveOscillator() = default;
		virtual hephaudio_float Oscillate(const size_t& frameIndex) const noexcept override;
	};
}