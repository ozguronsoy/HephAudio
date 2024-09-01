#pragma once
#include "HephCommonShared.h"
#include "Complex.h"
#include "Buffers/ComplexBuffer.h"
#include "Buffers/DoubleBuffer.h"

namespace HephCommon
{
	class Fourier final
	{
	private:
		static constexpr bool DIRECTION_FORWARD = true;
		static constexpr bool DIRECTION_BACKWARD = false;
	public:
		static ComplexBuffer FFT(const DoubleBuffer& doubleBuffer);
		static ComplexBuffer FFT(const DoubleBuffer& doubleBuffer, size_t fftSize);
		static void FFT(ComplexBuffer& complexBuffer);
		static void FFT(ComplexBuffer& complexBuffer, size_t fftSize);
		static void IFFT(DoubleBuffer& doubleBuffer, ComplexBuffer& complexBuffer);
		static void IFFT(ComplexBuffer& complexBuffer, bool scale);
		static double BinFrequencyToIndex(size_t sampleRate, size_t fftSize, double frequency);
		static double IndexToBinFrequency(size_t sampleRate, size_t fftSize, size_t index);
		static size_t CalculateFFTSize(size_t bufferSize);
		static DoubleBuffer Convolve(const DoubleBuffer& source, const DoubleBuffer& kernel);
		static DoubleBuffer Convolve(const DoubleBuffer& source, const DoubleBuffer& kernel, ConvolutionMode convolutionMode);
	private:
		static void ReverseBits(ComplexBuffer& complexBuffer, size_t fftSize);
		static void FFT_Internal(ComplexBuffer& complexBuffer, size_t fftSize, bool direction);
	};
}