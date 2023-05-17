#pragma once
#include "HephAudioFramework.h"
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
	class Oscillator
	{
	public:
		heph_float peakAmplitude;
		heph_float frequency;
		heph_float phase_rad;
		uint32_t sampleRate;
	protected:
		Oscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase, AngleUnit angleUnit);
	public:
		virtual ~Oscillator() = default;
		virtual heph_float Oscillate(size_t t_sample) const noexcept = 0;
		HephCommon::FloatBuffer GenerateBuffer() const noexcept;
		HephCommon::FloatBuffer GenerateBuffer(size_t frameCount) const noexcept;
		HephCommon::FloatBuffer GenerateBuffer(size_t frameIndex, size_t frameCount) const noexcept;
	};
}