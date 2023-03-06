#include "OscillatorBase.h"

namespace HephAudio
{
	OscillatorBase::OscillatorBase(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit)
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
		AudioBuffer buffer = AudioBuffer(frameCount, AudioFormatInfo(WAVE_FORMAT_HEPHAUDIO, 1, sizeof(HEPHAUDIO_DOUBLE) * 8, this->sampleRate));

		for (size_t i = 0; i < frameCount; i++)
		{
			buffer[i][0] = this->Oscillate(i + frameIndex);
		}

		return buffer;
	}
	const HEPHAUDIO_DOUBLE& OscillatorBase::GetPeakAmplitude() const noexcept
	{
		return this->peakAmplitude;
	}
	void OscillatorBase::SetPeakAmplitude(HEPHAUDIO_DOUBLE peakAmplitude) noexcept
	{
		this->peakAmplitude = peakAmplitude;
	}
	const HEPHAUDIO_DOUBLE& OscillatorBase::GetFrequency() const noexcept
	{
		return this->frequency;
	}
	void OscillatorBase::SetFrequency(HEPHAUDIO_DOUBLE frequency) noexcept
	{
		this->frequency = frequency;
		this->UpdateW();
	}
	const HEPHAUDIO_DOUBLE& OscillatorBase::GetPhase(AngleUnit angleUnit) const noexcept
	{
		return angleUnit == AngleUnit::Radian ? this->phase_rad : RadToDeg(this->phase_rad);
	}
	void OscillatorBase::SetPhase(HEPHAUDIO_DOUBLE phase, AngleUnit angleUnit) noexcept
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