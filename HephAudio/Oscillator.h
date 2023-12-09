#pragma once
#include "HephAudioFramework.h"
#include "../HephCommon/HeaderFiles/FloatBuffer.h"
#include <cmath>
#include <cinttypes>

namespace HephAudio
{
	class Oscillator
	{
	public:
		heph_float peakAmplitude;
		heph_float frequency;
		heph_float phase_rad;
		uint32_t sampleRate;
	protected:
		Oscillator();
		Oscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad);
	public:
		virtual ~Oscillator() = default;
		virtual heph_float operator[](size_t n) const noexcept = 0;
		HephCommon::FloatBuffer GenerateBuffer() const;
		HephCommon::FloatBuffer GenerateBuffer(size_t frameCount) const;
		HephCommon::FloatBuffer GenerateBuffer(size_t frameIndex, size_t frameCount) const;
	};
}