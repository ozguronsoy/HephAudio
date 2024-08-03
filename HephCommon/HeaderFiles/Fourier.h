#pragma once
#include "HephCommonShared.h"
#include "Complex.h"
#include "ComplexBuffer.h"
#include "FloatBuffer.h"

namespace HephCommon
{
	class Fourier final
	{
	public:
		static ComplexBuffer FFT(const FloatBuffer& floatBuffer);
		static ComplexBuffer FFT(const FloatBuffer& floatBuffer, size_t fftSize);
		static void FFT(ComplexBuffer& complexBuffer);
		static void FFT(ComplexBuffer& complexBuffer, size_t fftSize);
		static void IFFT(FloatBuffer& floatBuffer, ComplexBuffer& complexBuffer);
		static void IFFT(ComplexBuffer& complexBuffer, bool scale);
		static double BinFrequencyToIndex(size_t sampleRate, size_t fftSize, double frequency);
		static double IndexToBinFrequency(size_t sampleRate, size_t fftSize, size_t index);
		static size_t CalculateFFTSize(size_t bufferSize);
		static FloatBuffer Convolve(const FloatBuffer& source, const FloatBuffer& kernel);
		static FloatBuffer Convolve(const FloatBuffer& source, const FloatBuffer& kernel, ConvolutionMode convolutionMode);
	};
}