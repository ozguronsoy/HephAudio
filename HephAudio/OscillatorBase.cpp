#include "OscillatorBase.h"

namespace HephAudio
{
	OscillatorBase::OscillatorBase(hephaudio_float peakAmplitude, hephaudio_float frequency, uint32_t sampleRate, hephaudio_float phase, AngleUnit angleUnit)
		: peakAmplitude(peakAmplitude), sampleRate(sampleRate), frequency(frequency)
	{
		this->phase_rad = angleUnit == AngleUnit::Radian ? phase : DegToRad(phase);
	}
	AudioBuffer OscillatorBase::GenerateBuffer(size_t frameCount) const noexcept
	{
		return this->GenerateBuffer(0, frameCount);
	}
	AudioBuffer OscillatorBase::GenerateBuffer(size_t frameIndex, size_t frameCount) const noexcept
	{
		AudioBuffer buffer = AudioBuffer(frameCount, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, 1, sizeof(hephaudio_float) * 8, this->sampleRate));

		for (size_t i = 0; i < frameCount; i++)
		{
			buffer[i][0] = this->Oscillate(i + frameIndex);
		}

		return buffer;
	}
}