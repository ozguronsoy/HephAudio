#pragma once
#include "framework.h"
#include <cmath>
#include <cinttypes>

namespace HephAudio
{
	enum class AngleUnit : uint8_t
	{
		Degree,
		Radian
	};
	class OscillatorBase
	{
		friend class AudioProcessor;
	protected:
		HEPHAUDIO_DOUBLE peakAmplitude;
		HEPHAUDIO_DOUBLE frequency;
		HEPHAUDIO_DOUBLE phase_rad;
		HEPHAUDIO_DOUBLE sampleRate;
		HEPHAUDIO_DOUBLE w_sample;
	protected:
		OscillatorBase(const HEPHAUDIO_DOUBLE& peakAmplitude, const HEPHAUDIO_DOUBLE& frequency, const uint32_t& sampleRate, const HEPHAUDIO_DOUBLE& phase, const AngleUnit& angleUnit);
	public:
		virtual ~OscillatorBase() = default;
		virtual HEPHAUDIO_DOUBLE Oscillate(const size_t& frameIndex) const noexcept = 0;
		const HEPHAUDIO_DOUBLE& GetPeakAmplitude() const noexcept;
		void SetPeakAmplitude(HEPHAUDIO_DOUBLE peakAmplitude) noexcept;
		const HEPHAUDIO_DOUBLE& GetFrequency() const noexcept;
		void SetFrequency(HEPHAUDIO_DOUBLE frequency) noexcept;
		const HEPHAUDIO_DOUBLE& GetPhase(AngleUnit angleUnit) const noexcept;
		void SetPhase(HEPHAUDIO_DOUBLE phase, AngleUnit angleUnit) noexcept;
		const uint32_t& GetSampleRate() const noexcept;
		void SetSampleRate(uint32_t sampleRate) noexcept;
	protected:
		void UpdateW() noexcept;
	};
}