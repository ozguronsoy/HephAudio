#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class SawToothWaveOscillator : public OscillatorBase
	{
	public:
		SawToothWaveOscillator(const uint32_t& sampleRate);
		SawToothWaveOscillator(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit = AngleUnit::Radian);
		virtual ~SawToothWaveOscillator() = default;
		virtual hephaudio_float Oscillate(const size_t& frameIndex) const noexcept override;
	};
}