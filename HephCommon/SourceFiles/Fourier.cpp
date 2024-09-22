#include "Fourier.h"
#include "HephMath.h"

namespace Heph
{
	ComplexBuffer Fourier::FFT(const DoubleBuffer& doubleBuffer)
	{
		return Fourier::FFT(doubleBuffer, doubleBuffer.Size());
	}

	ComplexBuffer Fourier::FFT(const DoubleBuffer& doubleBuffer, size_t fftSize)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		ComplexBuffer complexBuffer(fftSize, BufferFlags::AllocUninitialized);

		const size_t minSize = HEPH_MATH_MIN(doubleBuffer.Size(), fftSize);
		const size_t maxSize = HEPH_MATH_MAX(doubleBuffer.Size(), fftSize);
		size_t i;

		for (i = 0; i < minSize; ++i)
		{
			complexBuffer[i] = Complex(doubleBuffer[i], 0);
		}
		for (; i < maxSize; ++i)
		{
			complexBuffer[i] = Complex();
		}

		Fourier::FFT_Internal(complexBuffer, fftSize, Fourier::DIRECTION_FORWARD);

		return complexBuffer;
	}

	void Fourier::FFT(ComplexBuffer& complexBuffer)
	{
		Fourier::FFT(complexBuffer, complexBuffer.Size());
	}

	void Fourier::FFT(ComplexBuffer& complexBuffer, size_t fftSize)
	{
		fftSize = Fourier::CalculateFFTSize(fftSize);
		complexBuffer.Resize(fftSize);
		Fourier::FFT_Internal(complexBuffer, fftSize, Fourier::DIRECTION_FORWARD);
	}

	void Fourier::IFFT(DoubleBuffer& doubleBuffer, ComplexBuffer& complexBuffer)
	{
		Fourier::FFT_Internal(complexBuffer, complexBuffer.Size(), Fourier::DIRECTION_BACKWARD);
		complexBuffer /= complexBuffer.Size();
		for (size_t i = 0; i < doubleBuffer.Size(); ++i)
		{
			doubleBuffer[i] = complexBuffer[i].real;
		}
	}

	void Fourier::IFFT(ComplexBuffer& complexBuffer, bool scale)
	{
		Fourier::FFT_Internal(complexBuffer, complexBuffer.Size(), Fourier::DIRECTION_BACKWARD);
		if (scale)
		{
			complexBuffer /= complexBuffer.Size();
		}
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

	DoubleBuffer Fourier::Convolve(const DoubleBuffer& source, const DoubleBuffer& kernel)
	{
		return Fourier::Convolve(source, kernel, ConvolutionMode::Full);
	}

	DoubleBuffer Fourier::Convolve(const DoubleBuffer& source, const DoubleBuffer& kernel, ConvolutionMode convolutionMode)
	{
		if (convolutionMode == ConvolutionMode::ValidPadding && kernel.Size() > source.Size())
		{
			return DoubleBuffer();
		}

		if (source.Size() == 0 || kernel.Size() == 0)
		{
			return DoubleBuffer();
		}

		size_t ySize = source.Size() + kernel.Size() - 1;
		const size_t fftSize = Fourier::CalculateFFTSize(ySize);

		ComplexBuffer tf(fftSize);
		{
			ComplexBuffer tfKernel(fftSize);

			for (size_t i = 0; i < source.Size(); i++)
			{
				tf[i].real = source[i];
			}

			for (size_t i = 0; i < kernel.Size(); i++)
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
			iStart = kernel.Size() / 2;
			ySize = source.Size();
			iEnd = ySize + iStart;
			break;
		case ConvolutionMode::ValidPadding:
			iStart = kernel.Size() - 1;
			ySize = source.Size() - kernel.Size() + 1;
			iEnd = ySize + iStart;
			break;
		case ConvolutionMode::Full:
		default:
			iStart = 0;
			ySize = source.Size() + kernel.Size() - 1;
			iEnd = ySize;
			break;
		}

		DoubleBuffer result(ySize);
		for (size_t i = iStart; i < iEnd; i++)
		{
			result[i - iStart] = tf[i].real / fftSize;
		}
		return result;
	}

	void Fourier::ReverseBits(ComplexBuffer& complexBuffer, size_t fftSize)
	{
		size_t j = 0;
		for (size_t i = 0; i < fftSize; ++i)
		{
			if (i < j)
			{
				std::swap(complexBuffer[i], complexBuffer[j]);
			}
			j ^= fftSize - fftSize / ((i ^ (i + 1)) + 1);
		}
	}

	void Fourier::FFT_Internal(ComplexBuffer& complexBuffer, size_t fftSize, bool direction)
	{
		Fourier::ReverseBits(complexBuffer, fftSize);

		Complex a, b, temp;
		size_t p, s1, s2, i, j;
		int sign = (direction == Fourier::DIRECTION_FORWARD) ? (1) : (-1);

		a.real = -1;

		for (p = fftSize, s1 = 1, s2 = 2; 
			p > 1; 
			p >>= 1, s1 <<= 1, s2 <<= 1)
		{
			b.real = 1;

			for (i = 0; i < s1; ++i)
			{
				for (j = i; j < fftSize; j += s2)
				{
					temp = b * complexBuffer[j + s1];
					complexBuffer[j + s1] = complexBuffer[j] - temp;
					complexBuffer[j] += temp;
				}
				b *= a;
			}

			a.imag = sign * sqrt((1.0 - a.real) * 0.5);
			a.real = sqrt((1.0 + a.real) * 0.5);
		}
	}
}