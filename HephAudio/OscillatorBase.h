#pragma once
#include "framework.h"
#include "FloatBuffer.h"
#include <cmath>
#include <cinttypes>

namespace HephAudio
{
	enum class AngleUnit : uint8_t
	{
		Degree,
		Radian
	};
	class OscillatorBase
	{
	public:
		hephaudio_float peakAmplitude;
		hephaudio_float frequency;
		hephaudio_float phase_rad;
		uint32_t sampleRate;
	protected:
		OscillatorBase(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase, AngleUnit angleUnit);
	public:
		virtual ~OscillatorBase() = default;
		virtual hephaudio_float Oscillate(size_t t_sample) const noexcept = 0;
		FloatBuffer GenerateBuffer() const noexcept;
		FloatBuffer GenerateBuffer(size_t frameCount) const noexcept;
		FloatBuffer GenerateBuffer(size_t frameIndex, size_t frameCount) const noexcept;
	};
}