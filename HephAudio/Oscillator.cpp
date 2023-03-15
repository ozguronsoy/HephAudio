#include "Oscillator.h"

namespace HephAudio
{
	Oscillator::Oscillator(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase, AngleUnit angleUnit)
		: peakAmplitude(peakAmplitude), sampleRate(sampleRate), frequency(frequency)
	{
		this->phase_rad = angleUnit == AngleUnit::Radian ? phase : DegToRad(phase);
	}
	FloatBuffer Oscillator::GenerateBuffer() const noexcept
	{
		return this->GenerateBuffer(ceil(this->sampleRate / this->frequency));
	}
	FloatBuffer Oscillator::GenerateBuffer(size_t frameCount) const noexcept
	{
		FloatBuffer buffer = FloatBuffer(frameCount);
		for (size_t i = 0; i < frameCount; i++)
		{
			buffer[i] = this->Oscillate(i);
		}
		return buffer;
	}
	FloatBuffer Oscillator::GenerateBuffer(size_t frameIndex, size_t frameCount) const noexcept
	{
		FloatBuffer buffer = FloatBuffer(frameCount);
		for (size_t i = 0; i < frameCount; i++)
		{
			buffer[i] = this->Oscillate(i + frameIndex);
		}
		return buffer;
	}
}