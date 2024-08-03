#pragma once
#include "HephAudioShared.h"
#include "FloatBuffer.h"
#include <cmath>
#include <cinttypes>

namespace HephAudio
{
	class Oscillator
	{
	public:
		double peakAmplitude;
		double frequency;
		double phase_rad;
		uint32_t sampleRate;
	protected:
		Oscillator();
		Oscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad);
	public:
		virtual ~Oscillator() = default;
		virtual double operator[](size_t n) const = 0;
		HephCommon::FloatBuffer GenerateBuffer() const;
		HephCommon::FloatBuffer GenerateBuffer(size_t frameCount) const;
		HephCommon::FloatBuffer GenerateBuffer(size_t frameIndex, size_t frameCount) const;
	};
}