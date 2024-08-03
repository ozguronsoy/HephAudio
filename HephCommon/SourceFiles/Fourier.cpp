#include "Fourier.h"
#include <../../dependencies/pocketfft/pocketfft_hdronly.h>
#include <complex>

namespace HephCommon
{
	ComplexBuffer Fourier::FFT(const FloatBuffer& floatBuffer)
	{
		return Fourier::FFT(floatBuffer, floatBuffer.FrameCount());
	}
	ComplexBuffer Fourier::FFT(const FloatBuffer& floatBuffer, size_t fftSize)
	{
		static pocketfft::stride_t stride_in{ sizeof(double) };
		static pocketfft::stride_t stride_out{ sizeof(Complex) };
		static pocketfft::shape_t axes{ 0 };

		fftSize = Fourier::CalculateFFTSize(fftSize);
		ComplexBuffer complexBuffer = ComplexBuffer(fftSize);
		pocketfft::shape_t shape_in{ fftSize };
		std::complex<double>* pComplexBuffer = (std::complex<double>*)complexBuffer.Begin();

		if (floatBuffer.FrameCount() != fftSize)
		{
			FloatBuffer tempBuffer = floatBuffer;
			tempBuffer.Resize(fftSize);
			const double* pRealBuffer = (const double*)tempBuffer.Begin();
			pocketfft::r2c(shape_in, stride_in, stride_out, axes, true, pRealBuffer, pComplexBuffer, (double)1.0);
		}
		else
		{
			const double* pRealBuffer = (const double*)floatBuffer.Begin();
			pocketfft::r2c(shape_in, stride_in, stride_out, axes, true, pRealBuffer, pComplexBuffer, (double)1.0);
		}

		return complexBuffer;
	}
	void Fourier::FFT(ComplexBuffer& complexBuffer)
	{
		Fourier::FFT(complexBuffer, complexBuffer.FrameCount());
	}
	void Fourier::FFT(ComplexBuffer& complexBuffer, size_t fftSize)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		complexBuffer.Resize(fftSize);

		static pocketfft::stride_t stride_in{ sizeof(Complex) };
		static pocketfft::stride_t stride_out{ sizeof(Complex) };
		static pocketfft::shape_t axes{ 0 };

		pocketfft::shape_t shape_in{ fftSize };
		std::complex<double>* pComplexBuffer = (std::complex<double>*)complexBuffer.Begin();
		pocketfft::c2c(shape_in, stride_in, stride_out, axes, true, (const std::complex<double>*)pComplexBuffer, pComplexBuffer, (double)1.0);
	}
	void Fourier::IFFT(FloatBuffer& floatBuffer, ComplexBuffer& complexBuffer)
	{
		static pocketfft::stride_t stride_in{ sizeof(Complex) };
		static pocketfft::stride_t stride_out{ sizeof(double) };
		static pocketfft::shape_t axes{ 0 };

		const size_t fftSize = complexBuffer.FrameCount();
		pocketfft::shape_t shape_in{ fftSize };
		double* pRealBuffer = (double*)floatBuffer.Begin();
		const std::complex<double>* pComplexBuffer = (std::complex<double>*)complexBuffer.Begin();

		pocketfft::c2r(shape_in, stride_in, stride_out, axes, false, pComplexBuffer, pRealBuffer, (double)1.0 / (double)fftSize);
	}
	void Fourier::IFFT(ComplexBuffer& complexBuffer, bool scale)
	{
		static pocketfft::stride_t stride_in{ sizeof(Complex) };
		static pocketfft::stride_t stride_out{ sizeof(Complex) };
		static pocketfft::shape_t axes{ 0 };

		const size_t fftSize = complexBuffer.FrameCount();
		pocketfft::shape_t shape_in{ fftSize };
		std::complex<double>* pComplexBuffer = (std::complex<double>*)complexBuffer.Begin();

		pocketfft::c2c(shape_in, stride_in, stride_out, axes, false, (const std::complex<double>*)pComplexBuffer, pComplexBuffer,
			scale ? ((double)1.0 / (double)fftSize) : (double)1.0);
	}
	double Fourier::BinFrequencyToIndex(size_t sampleRate, size_t fftSize, double frequency)
	{
		return round(frequency * fftSize / sampleRate);
	}
	double Fourier::IndexToBinFrequency(size_t sampleRate, size_t fftSize, size_t index)
	{
		return (double)index * sampleRate / fftSize;
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
				tf[i].real(source[i]);
			}

			for (size_t i = 0; i < kernel.FrameCount(); i++)
			{
				tfKernel[i].real(kernel[i]);
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
			result[i - iStart] = tf[i].real() / fftSize;
		}
		return result;
	}
}