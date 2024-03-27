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
	FloatBuffer Fourier::Convolve(const FloatBuffer& source, const FloatBuffer& kernel)
	{
		return Fourier::Convolve(source, kernel, ConvolutionMode::Full);
	}
	FloatBuffer Fourier::Convolve(const FloatBuffer& source, const FloatBuffer& kernel, ConvolutionMode convolutionMode)
	{
		if (convolutionMode == ConvolutionMode::ValidPadding && kernel.FrameCount() > source.FrameCount())
		{
			return FloatBuffer();
		}

		if (source.FrameCount() == 0 || kernel.FrameCount() == 0)
		{
			return FloatBuffer();
		}

		size_t yFrameCount = source.FrameCount() + kernel.FrameCount() - 1;
		const size_t fftSize = Fourier::CalculateFFTSize(yFrameCount);

		ComplexBuffer tf(fftSize);
		{
			ComplexBuffer tfKernel(fftSize);

			for (size_t i = 0; i < source.FrameCount(); i++)
			{
				tf[i].real = source[i];
			}

			for (size_t i = 0; i < kernel.FrameCount(); i++)
			{
				tfKernel[i].real = kernel[i];
			}

			Fourier::FFT(tf);
			Fourier::FFT(tfKernel);

			tf *= tfKernel;
		}

		Fourier::IFFT(tf, false);

		size_t iStart = 0;
		size_t iEnd = 0;
		switch (convolutionMode)
		{
		case ConvolutionMode::Central:
			iStart = kernel.FrameCount() / 2;
			yFrameCount = source.FrameCount();
			iEnd = yFrameCount + iStart;
			break;
		case ConvolutionMode::ValidPadding:
			iStart = kernel.FrameCount() - 1;
			yFrameCount = source.FrameCount() - kernel.FrameCount() + 1;
			iEnd = yFrameCount + iStart;
			break;
		case ConvolutionMode::Full:
		default:
			iStart = 0;
			yFrameCount = source.FrameCount() + kernel.FrameCount() - 1;
			iEnd = yFrameCount;
			break;
		}

		FloatBuffer result(yFrameCount);
		for (size_t i = iStart; i < iEnd; i++)
		{
			result[i - iStart] = tf[i].real / fftSize;
		}
		return result;
	}
	void Fourier::FFT_Internal(ComplexBuffer& complexBuffer, size_t fftSize, bool isForward)
	{
		static pocketfft::stride_t stride_in{ sizeof(heph_float) };
		static pocketfft::stride_t stride_out{ sizeof(Complex) };
		static pocketfft::shape_t axes{ 0 };

		pocketfft::shape_t shape_in{ fftSize };
		std::complex<heph_float>* pComplexBuffer = (std::complex<heph_float>*)complexBuffer.Begin();
		pocketfft::c2c(shape_in, stride_out, stride_out, axes, isForward, pComplexBuffer, pComplexBuffer, (heph_float)1.0);
	}
}