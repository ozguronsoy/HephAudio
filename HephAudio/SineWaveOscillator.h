#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class SineWaveOscillator : public OscillatorBase
	{
	public:
		SineWaveOscillator(const uint32_t& sampleRate);
		SineWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit = AngleUnit::Radian);
		virtual ~SineWaveOscillator() = default;
		virtual hephaudio_float Oscillate(const size_t& frameIndex) const noexcept override;
	};
}