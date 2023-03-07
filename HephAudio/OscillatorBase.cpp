#include "OscillatorBase.h"

namespace HephAudio
{
	OscillatorBase::OscillatorBase(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit)
	{
		this->SetPeakAmplitude(peakAmplitude);
		this->SetSampleRate(sampleRate);
		this->SetFrequency(frequency);
		this->SetPhase(phase, angleUnit);
	}
	AudioBuffer OscillatorBase::GenerateBuffer(size_t frameCount) const noexcept
	{
		return this->GenerateBuffer(0, frameCount);
	}
	AudioBuffer OscillatorBase::GenerateBuffer(const size_t& frameIndex, size_t frameCount) const noexcept
	{
		AudioBuffer buffer = AudioBuffer(frameCount, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, 1, sizeof(hephaudio_float) * 8, this->sampleRate));

		for (size_t i = 0; i < frameCount; i++)
		{
			buffer[i][0] = this->Oscillate(i + frameIndex);
		}

		return buffer;
	}
	const hephaudio_float& OscillatorBase::GetPeakAmplitude() const noexcept
	{
		return this->peakAmplitude;
	}
	void OscillatorBase::SetPeakAmplitude(hephaudio_float peakAmplitude) noexcept
	{
		this->peakAmplitude = peakAmplitude;
	}
	const hephaudio_float& OscillatorBase::GetFrequency() const noexcept
	{
		return this->frequency;
	}
	void OscillatorBase::SetFrequency(hephaudio_float frequency) noexcept
	{
		this->frequency = frequency;
		this->UpdateW();
	}
	const hephaudio_float& OscillatorBase::GetPhase(AngleUnit angleUnit) const noexcept
	{
		return angleUnit == AngleUnit::Radian ? this->phase_rad : RadToDeg(this->phase_rad);
	}
	void OscillatorBase::SetPhase(hephaudio_float phase, AngleUnit angleUnit) noexcept
	{
		this->phase_rad = angleUnit == AngleUnit::Radian ? phase : DegToRad(phase);
	}
	const uint32_t& OscillatorBase::GetSampleRate() const noexcept
	{
		return this->sampleRate;
	}
	void OscillatorBase::SetSampleRate(uint32_t sampleRate) noexcept
	{
		this->sampleRate = sampleRate;
		this->UpdateW();
	}
	void OscillatorBase::UpdateW() noexcept
	{
		this->w_sample = 2.0 * PI * this->frequency / this->sampleRate;
	}
}