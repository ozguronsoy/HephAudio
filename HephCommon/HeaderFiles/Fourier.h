#pragma once
#include "HephShared.h"
#include "Complex.h"
#include "Buffers/ComplexBuffer.h"
#include "Buffers/DoubleBuffer.h"

/** @file */

namespace Heph
{
	/**
	 * @brief class for calculating FFT and Convolution.
	 * @note this class cannot be instantiated.
	 */
	class HEPH_API Fourier final
	{
	private:
		static constexpr bool DIRECTION_FORWARD = true;
		static constexpr bool DIRECTION_BACKWARD = false;

	public:
		Fourier() = delete;
		Fourier(const Fourier&) = delete;
		Fourier& operator=(const Fourier&) = delete;

	public:
		/**
		 * computes the forward Fast Fourier Transform.
		 * 
		 * @param doubleBuffer real data in time domain.
		 * @return complex data in frequency domain.
		 */
		static ComplexBuffer FFT(const DoubleBuffer& doubleBuffer);

		/**
		 * computes the forward Fast Fourier Transform.
		 * 
		 * @param doubleBuffer real data in time domain.
		 * @param fftSize size of the FFT. Must be a power of 2, if not the closest power of 2 will be used.
		 * @return complex data in frequency domain.
		 */
		static ComplexBuffer FFT(const DoubleBuffer& doubleBuffer, size_t fftSize);

		/**
		 * computes the forward Fast Fourier Transform.
		 * 
		 * @param complexBuffer contains the time domain data in input, and frequency domain data in output.
		 */
		static void FFT(ComplexBuffer& complexBuffer);

		/**
		 * computes the forward Fast Fourier Transform.
		 * 
		 * @param complexBuffer contains the time domain data in input, and frequency domain data in output.
		 * @param fftSize size of the FFT. Must be a power of 2, if not the closest power of 2 will be used.
		 */
		static void FFT(ComplexBuffer& complexBuffer, size_t fftSize);

		/**
		 * computes the inverse Fast Fourier Transform.
		 * 
		 * @param doubleBuffer real data in time domain.
		 * @param complexBuffer complex data in frequency domain.
		 */
		static void IFFT(DoubleBuffer& doubleBuffer, ComplexBuffer& complexBuffer);

		/**
		 * computes the inverse Fast Fourier Transform.
		 * 
		 * @param complexBuffer contains the frequency domain data in input, and time domain data in output.
		 * @param scale indicates whether to divide the output by fftSize.
		 */
		static void IFFT(ComplexBuffer& complexBuffer, bool scale);

		/**
		 * computes the corresponding bin index to the provided frequency.
		 * 
		 * @param sampleRate the sampling rate of the transformed signal.
		 */
		static double BinFrequencyToIndex(size_t sampleRate, size_t fftSize, double frequency);

		/**
		 * computes the corresponding frequency to the provided bin index.
		 * 
		 * @param sampleRate the sampling rate of the transformed signal.
		 */
		static double IndexToBinFrequency(size_t sampleRate, size_t fftSize, size_t index);

		/**
		 * gets the closest power of 2 that is greater than or equal to the bufferSize.
		 * 
		 */
		static size_t CalculateFFTSize(size_t bufferSize);

		/**
		 * computes convolution in Full mode.
		 * 
		 * @return the convolution result.
		 */
		static DoubleBuffer Convolve(const DoubleBuffer& source, const DoubleBuffer& kernel);

		/**
		 * computes convolution.
		 * 
		 * @return the convolution result.
		 */
		static DoubleBuffer Convolve(const DoubleBuffer& source, const DoubleBuffer& kernel, ConvolutionMode convolutionMode);

	private:
		static void ReverseBits(ComplexBuffer& complexBuffer, size_t fftSize);
		static void FFT_Internal(ComplexBuffer& complexBuffer, size_t fftSize, bool direction);
	};
}