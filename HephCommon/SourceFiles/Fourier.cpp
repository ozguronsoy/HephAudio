#include "Fourier.h"

namespace HephCommon
{
	ComplexBuffer Fourier::FFT(const FloatBuffer& floatBuffer)
	{
		return Fourier::FFT(floatBuffer, floatBuffer.FrameCount());
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
			return 1 << (size_t)ceil(log2(bufferSize)); // smallest power of 2 thats greater than nSample
		}
		return bufferSize;
	}
	void Fourier::ReverseBits(ComplexBuffer& complexBuffer, size_t fftSize)
	{
		size_t j = 0;
		for (size_t i = 0; i < fftSize; i++)
		{
			if (i < j)
			{
				const Complex temp = complexBuffer[j];
				complexBuffer[j] = complexBuffer[i];
				complexBuffer[i] = temp;
			}
			j ^= fftSize - fftSize / ((i ^ (i + 1)) + 1);
		}
	}
	void Fourier::Fourier::FFT_Internal(ComplexBuffer& complexBuffer, size_t fftSize, bool isForward)
	{
		Fourier::ReverseBits(complexBuffer, fftSize);
		const size_t p = log2(fftSize);
		Complex a = Complex(-1.0, 0.0);
		for (size_t i = 0; i < p; i++)
		{
			const size_t s = (1 << i);
			const size_t s2 = s << 1;
			Complex b = Complex(1.0, 0.0);
			for (size_t j = 0; j < s; j++)
			{
				for (size_t k = j; k < fftSize; k += s2)
				{
					const Complex temp = b * complexBuffer[k + s];
					complexBuffer[k + s] = complexBuffer[k] - temp;
					complexBuffer[k] += temp;
				}
				b *= a;
			}
			a.imaginary = isForward ? -sqrt((1.0 - a.real) * 0.5) : sqrt((1.0 - a.real) * 0.5);
			a.real = sqrt((1.0 + a.real) * 0.5);
		}
	}
}