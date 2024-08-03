#include "Oscillators/SawToothWaveOscillator.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	SawToothWaveOscillator::SawToothWaveOscillator() : Oscillator() {}
	SawToothWaveOscillator::SawToothWaveOscillator(uint32_t sampleRate) : Oscillator(0.05, 1500.0, sampleRate, 0) {}
	SawToothWaveOscillator::SawToothWaveOscillator(double peakAmplitude, double frequency, uint32_t sampleRate, double phase_rad)
		: Oscillator(peakAmplitude, frequency, sampleRate, phase_rad) {}
	double SawToothWaveOscillator::operator[](size_t n) const
	{
		const double t_second = (double)n / this->sampleRate + HEPH_MATH_RAD_TO_DEG(this->phase_rad);
		const double ft = this->frequency * t_second;
		return this->peakAmplitude * 2.0 * (this->frequency * t_second - round(this->frequency * t_second));
	}
}