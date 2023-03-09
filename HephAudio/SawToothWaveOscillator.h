#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class SawToothWaveOscillator : public OscillatorBase
	{
	public:
		SawToothWaveOscillator(uint32_t sampleRate);
		SawToothWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase = 0.0, AngleUnit angleUnit = AngleUnit::Radian);
		virtual ~SawToothWaveOscillator() = default;
		virtual hephaudio_float Oscillate(size_t t_sample) const noexcept override;
	};
}