#include "Oscillators/Oscillator.h"

using namespace Heph;

namespace HephAudio
{
	Oscillator::Oscillator() : peakAmplitude(1), sampleRate(48000), frequency(1), phase_rad(0) {}
	
	Oscillator::Oscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad)
		: peakAmplitude(peakAmplitude), sampleRate(sampleRate), frequency(frequency), phase_rad(phase_rad) { }

	DoubleBuffer Oscillator::GenerateBuffer() const
	{
		return this->GenerateBuffer(std::ceil(this->sampleRate / this->frequency));
	}
	DoubleBuffer Oscillator::GenerateBuffer(size_t size) const
	{
		DoubleBuffer buffer(size, BufferFlags::AllocUninitialized);
		for (size_t i = 0; i < size; ++i)
		{
			buffer[i] = (*this)[i];
		}
		return buffer;
	}
	DoubleBuffer Oscillator::GenerateBuffer(size_t index, size_t size) const
	{
		DoubleBuffer buffer(size, BufferFlags::AllocUninitialized);
		for (size_t i = 0; i < size; ++i)
		{
			buffer[i] = (*this)[i + index];
		}
		return buffer;
	}
}