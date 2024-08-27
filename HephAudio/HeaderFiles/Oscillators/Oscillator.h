#pragma once
#include "HephAudioShared.h"
#include "Buffers/DoubleBuffer.h"
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
		HephCommon::DoubleBuffer GenerateBuffer() const;
		HephCommon::DoubleBuffer GenerateBuffer(size_t size) const;
		HephCommon::DoubleBuffer GenerateBuffer(size_t index, size_t size) const;
	};
}