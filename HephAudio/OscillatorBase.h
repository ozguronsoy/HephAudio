#pragma once
#include "framework.h"
#include "AudioBuffer.h"
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
		hephaudio_float peakAmplitude;
		hephaudio_float frequency;
		hephaudio_float phase_rad;
		uint32_t sampleRate;
		hephaudio_float w_sample;
	protected:
		OscillatorBase(const hephaudio_float& peakAmplitude, const hephaudio_float& frequency, const uint32_t& sampleRate, const hephaudio_float& phase, const AngleUnit& angleUnit);
	public:
		virtual ~OscillatorBase() = default;
		virtual hephaudio_float Oscillate(const size_t& frameIndex) const noexcept = 0;
		virtual AudioBuffer GenerateBuffer(size_t frameCount) const noexcept;
		virtual AudioBuffer GenerateBuffer(const size_t& frameIndex, size_t frameCount) const noexcept;
		virtual const hephaudio_float& GetPeakAmplitude() const noexcept;
		virtual void SetPeakAmplitude(hephaudio_float peakAmplitude) noexcept;
		virtual const hephaudio_float& GetFrequency() const noexcept;
		virtual void SetFrequency(hephaudio_float frequency) noexcept;
		virtual const hephaudio_float& GetPhase(AngleUnit angleUnit) const noexcept;
		virtual void SetPhase(hephaudio_float phase, AngleUnit angleUnit) noexcept;
		virtual const uint32_t& GetSampleRate() const noexcept;
		virtual void SetSampleRate(uint32_t sampleRate) noexcept;
	protected:
		virtual void UpdateW() noexcept;
	};
}