#pragma once
#include "framework.h"
#include "Complex.h"
#include "ComplexBuffer.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	class Fourier final
	{
	public:
		static ComplexBuffer FFT_Forward(const AudioBuffer& audioBuffer);
		static ComplexBuffer FFT_Forward(const AudioBuffer& audioBuffer, size_t fftSize);
		static void FFT_Inverse(AudioBuffer& audioBuffer, ComplexBuffer& complexBuffer);
		static double Magnitude(Complex sample);
		static double MagnitudeSquared(Complex sample);
		static double Phase(Complex sample, bool isDegree);
		static double Decibels(Complex sample);
		static double FrequencyToIndex(size_t sampleRate, size_t fftSize, double frequency);
		static double IndexToFrequency(size_t sampleRate, size_t fftSize, size_t index);
		static size_t CalculateFFTSize(size_t bufferSize);
	private:
		static void ReverseBits(ComplexBuffer& complexBuffer, const size_t& fftSize);
		static void FFT(ComplexBuffer& complexBuffer, const size_t& fftSize, bool isForward);
	};
}