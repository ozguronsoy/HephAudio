#include "Oscillators/Oscillator.h"

using namespace HephCommon;

namespace HephAudio
{
	Oscillator::Oscillator() : peakAmplitude(1), sampleRate(48000), frequency(1), phase_rad(0) {}
	Oscillator::Oscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad)
		: peakAmplitude(peakAmplitude), sampleRate(sampleRate), frequency(frequency), phase_rad(phase_rad) { }
	FloatBuffer Oscillator::GenerateBuffer() const
	{
		return this->GenerateBuffer(ceil(this->sampleRate / this->frequency));
	}
	FloatBuffer Oscillator::GenerateBuffer(size_t frameCount) const
	{
		FloatBuffer buffer(frameCount);
		for (size_t i = 0; i < frameCount; i++)
		{
			buffer[i] = (*this)[i];
		}
		return buffer;
	}
	FloatBuffer Oscillator::GenerateBuffer(size_t frameIndex, size_t frameCount) const
	{
		FloatBuffer buffer(frameCount);
		for (size_t i = 0; i < frameCount; i++)
		{
			buffer[i] = (*this)[i + frameIndex];
		}
		return buffer;
	}
}