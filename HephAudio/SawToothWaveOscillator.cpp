#include "SawToothWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	SawToothWaveOscillator::SawToothWaveOscillator(uint32_t sampleRate) : Oscillator(0.05, 1500.0, sampleRate, 0.0, AngleUnit::Radian) {}
	SawToothWaveOscillator::SawToothWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase, AngleUnit angleUnit)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase, angleUnit) {}
	heph_float SawToothWaveOscillator::Oscillate(size_t t_sample) const noexcept
	{
		const heph_float t_second = (heph_float)t_sample / this->sampleRate + Math::RadToDeg(this->phase_rad);
		const heph_float ft = this->frequency * t_second;
		return this->peakAmplitude * 2.0 * (this->frequency * t_second - round(this->frequency * t_second));
	}
}