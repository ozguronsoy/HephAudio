#include "Fourier.h"
#include "AudioException.h"

namespace HephAudio
{
	void Fourier::ComplexBufferToAudioBuffer(AudioBuffer& audioBuffer, const ComplexBuffer& complexBuffer)
	{
		const size_t bufferSize = audioBuffer.FrameCount();
		for (size_t i = 0; i < bufferSize; i++)
		{
			audioBuffer.Set(complexBuffer[i].real, i, 0);
		}
	}
	ComplexBuffer Fourier::FFT_Forward(const AudioBuffer& audioBuffer)
	{
		return FFT_Forward(audioBuffer, audioBuffer.FrameCount());
	}
	ComplexBuffer Fourier::FFT_Forward(const AudioBuffer& audioBuffer, size_t fftSize)
	{
		fftSize = CalculateFFTSize(fftSize);
		ComplexBuffer complexBuffer = ComplexBuffer(fftSize);
		for (size_t i = 0; i < audioBuffer.FrameCount(); i++)
		{
			complexBuffer[i].real = audioBuffer.Get(i, 0);
		}
		FFT(complexBuffer, fftSize, true);
		return complexBuffer;
	}
	void Fourier::FFT_Inverse(AudioBuffer& audioBuffer, ComplexBuffer& complexBuffer)
	{
		FFT(complexBuffer, CalculateFFTSize(complexBuffer.FrameCount()), false);
		ComplexBufferToAudioBuffer(audioBuffer, complexBuffer);
	}
	double Fourier::Magnitude(Complex sample)
	{
		return sqrt(MagnitudeSquared(sample));
	}
	double Fourier::MagnitudeSquared(Complex sample)
	{
		return pow(sample.real, 2) + pow(sample.imaginary, 2);
	}
	double Fourier::Phase(Complex sample, bool isDegree)
	{
		if (isDegree)
		{
			return atan2(sample.imaginary, sample.real) * 180.0 / PI;
		}
		return atan2(sample.imaginary, sample.real);
	}
	double Fourier::Decibels(Complex sample)
	{
		if (sample.real == 0 && sample.imaginary == 0)
		{
			return 0;
		}
		return 10.0 * log10(MagnitudeSquared(sample));
	}
	double Fourier::FrequencyToIndex(size_t sampleRate, size_t fftSize, double frequency)
	{
		return round(frequency * fftSize / sampleRate);
	}
	double Fourier::IndexToFrequency(size_t sampleRate, size_t fftSize, size_t index)
	{
		return round((double)index * sampleRate / fftSize);
	}
	size_t Fourier::CalculateFFTSize(size_t bufferSize)
	{
		if (!(bufferSize > 0 && !(bufferSize & (bufferSize - 1)))) // if not power of 2
		{
			return 1 << (size_t)ceil(log2(bufferSize)); // smallest power of 2 thats greater than nSample
		}
		return bufferSize;
	}
	void Fourier::ReverseBits(ComplexBuffer& complexBuffer, const size_t& fftSize)
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
	void Fourier::FFT(ComplexBuffer& complexBuffer, size_t fftSize, const bool isForward)
	{
		ReverseBits(complexBuffer, fftSize);
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
		if (!isForward)
		{
			for (size_t i = 0; i < fftSize; i++)
			{
				complexBuffer[i] /= fftSize;
			}
		}
	}
}