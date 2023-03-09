#pragma once
#include "framework.h"
#include "OscillatorBase.h"

namespace HephAudio
{
	class SineWaveOscillator : public OscillatorBase
	{
	public:
		SineWaveOscillator(uint32_t sampleRate);
		SineWaveOscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase = 0.0, AngleUnit angleUnit = AngleUnit::Radian);
		virtual ~SineWaveOscillator() = default;
		virtual hephaudio_float Oscillate(size_t t_sample) const noexcept override;
	};
}