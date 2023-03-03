#include "OscillatorBase.h"

namespace HephAudio
{
	OscillatorBase::OscillatorBase(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit)
	{
		this->SetPeakAmplitude(peakAmplitude);
		this->SetFrequency(frequency);
		this->SetSampleRate(sampleRate);
		this->SetPhase(phase, angleUnit);
	}
	const HEPHAUDIO_DOUBLE& OscillatorBase::GetPeakAmplitude() const noexcept
	{
		return this->peakAmplitude;
	}
	void OscillatorBase::SetPeakAmplitude(HEPHAUDIO_DOUBLE peakAmplitude) noexcept
	{
		this->peakAmplitude = abs(peakAmplitude);
		if (this->peakAmplitude > 1.0)
		{
			this->peakAmplitude = 1.0;
		}
	}
	const HEPHAUDIO_DOUBLE& OscillatorBase::GetFrequency() const noexcept
	{
		return this->frequency;
	}
	void OscillatorBase::SetFrequency(HEPHAUDIO_DOUBLE frequency) noexcept
	{
		this->frequency = frequency;
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
	}
}