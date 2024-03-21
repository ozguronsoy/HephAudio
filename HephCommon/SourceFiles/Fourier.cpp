#include "Fourier.h"
#include "../pocketfft/pocketfft_hdronly.h"
#include <complex>

namespace HephCommon
{
	ComplexBuffer Fourier::FFT(const FloatBuffer& floatBuffer)
	{
		return Fourier::FFT(floatBuffer, Fourier::CalculateFFTSize(floatBuffer.FrameCount()));
	}
	ComplexBuffer Fourier::FFT(const FloatBuffer& floatBuffer, size_t fftSize)
	{
		ComplexBuffer complexBuffer = ComplexBuffer(fftSize);
		for (size_t i = 0; i < floatBuffer.FrameCount(); i++)
		{
			complexBuffer[i].real = floatBuffer[i];
		}
		Fourier::FFT_Internal(complexBuffer, fftSize, true);
		return complexBuffer;
	}
	void Fourier::FFT(ComplexBuffer& complexBuffer)
	{
		Fourier::FFT(complexBuffer, complexBuffer.FrameCount());
	}
	void Fourier::FFT(ComplexBuffer& complexBuffer, size_t fftSize)
	{
		complexBuffer.Resize(fftSize);
		Fourier::FFT_Internal(complexBuffer, fftSize, true);
	}
	void Fourier::IFFT(FloatBuffer& floatBuffer, ComplexBuffer& complexBuffer)
	{
		Fourier::FFT_Internal(complexBuffer, complexBuffer.FrameCount(), false);
		for (size_t i = 0; i < floatBuffer.FrameCount(); i++)
		{
			complexBuffer[i] /= complexBuffer.FrameCount();
			floatBuffer[i] = complexBuffer[i].real;
		}
	}
	void Fourier::IFFT(ComplexBuffer& complexBuffer, bool scale)
	{
		Fourier::FFT_Internal(complexBuffer, complexBuffer.FrameCount(), false);
		if (scale)
		{
			complexBuffer /= complexBuffer.FrameCount();
		}
	}
	heph_float Fourier::BinFrequencyToIndex(size_t sampleRate, size_t fftSize, heph_float frequency)
	{
		return round(frequency * fftSize / sampleRate);
	}
	heph_float Fourier::IndexToBinFrequency(size_t sampleRate, size_t fftSize, size_t index)
	{
		return (heph_float)index * sampleRate / fftSize;
	}
	size_t Fourier::CalculateFFTSize(size_t bufferSize)
	{
		if (!(bufferSize > 0 && !(bufferSize & (bufferSize - 1)))) // if not power of 2
		{
			return (size_t)1 << (size_t)ceil(log2(bufferSize)); // smallest power of 2 thats greater than nSample
		}
		return bufferSize;
	}
	void Fourier::Fourier::FFT_Internal(ComplexBuffer& complexBuffer, size_t fftSize, bool isForward)
	{
		static pocketfft::stride_t stride_in{ sizeof(heph_float) };
		static pocketfft::stride_t stride_out{ sizeof(Complex) };
		static pocketfft::shape_t axes{ 0 };

		pocketfft::shape_t shape_in{ fftSize };
		std::complex<heph_float>* pComplexBuffer = (std::complex<heph_float>*)complexBuffer.Begin();
		pocketfft::c2c(shape_in, stride_out, stride_out, axes, isForward, pComplexBuffer, pComplexBuffer, 1.0_hf);
	}
}