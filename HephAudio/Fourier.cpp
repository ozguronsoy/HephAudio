#include "Fourier.h"
#include "AudioException.h"

namespace HephAudio
{
	void Fourier::ComplexBufferToAudioBuffer(AudioBuffer& audioBuffer, const ComplexBuffer& complexBuffer)
	{
		const size_t bufferSize = audioBuffer.FrameCount();
		for (size_t i = 0; i < bufferSize; i++)
		{
			audioBuffer.Set(complexBuffer.at(i).real, i, 0);
		}
	}
	ComplexBuffer Fourier::FFT_Forward(const AudioBuffer& audioBuffer)
	{
		return FFT_Forward(audioBuffer, audioBuffer.FrameCount());
	}
	ComplexBuffer Fourier::FFT_Forward(const AudioBuffer& audioBuffer, size_t fftSize)
	{
		fftSize = CalculateFFTSize(fftSize);
		const size_t p = log2f(fftSize);
		ComplexBuffer complexBuffer = ComplexBuffer(fftSize, Complex(0.0, 0.0));
		for (size_t i = 0; i < audioBuffer.FrameCount(); i++)
		{
			complexBuffer.at(i).real = audioBuffer.Get(i, 0);
		}
		FFT(complexBuffer, p, true);
		return complexBuffer;
	}
	void Fourier::FFT_Inverse(AudioBuffer& audioBuffer, ComplexBuffer& complexBuffer)
	{
		FFT(complexBuffer, log2f(complexBuffer.size()), false);
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
		return frequency * fftSize / sampleRate;
	}
	size_t Fourier::CalculateFFTSize(size_t bufferSize)
	{
		if (!(bufferSize > 0 && !(bufferSize & (bufferSize - 1)))) // if not power of 2
		{
			return 1 << (size_t)ceil(log2f(bufferSize)); // smallest power of 2 thats greater than nSample
		}
		return bufferSize;
	}
	void Fourier::ReverseBits(ComplexBuffer& complexBuffer, const size_t& p)
	{
		size_t k = 0;
		ComplexBuffer resultBuffer(complexBuffer.size(), Complex());
		for (size_t i = 0; i < complexBuffer.size(); i++)
		{
			k = 0;
			for (size_t j = 0; j < p; j++)
			{
				const size_t jp = 1 << j;
				if ((i & jp) == jp)
				{
					k += (1 << (p - j - 1));
				}
				resultBuffer.at(i) = complexBuffer.at(k);
			}
		}
		complexBuffer = resultBuffer;
	}
	void Fourier::FFT(ComplexBuffer& complexBuffer, const size_t& p, const bool isForward)
	{
		ReverseBits(complexBuffer, p);
		const size_t nSample = complexBuffer.size();
		Complex a = Complex(-1.0, 0.0);
		for (size_t i = 0; i < p; i++)
		{
			const size_t s = (1 << i);
			const size_t s2 = s << 1;
			Complex b = Complex(1.0, 0.0);
			for (size_t j = 0; j < s; j++)
			{
				for (size_t k = j; k < nSample; k += s2)
				{
					const Complex temp = b * complexBuffer.at(k + s);
					complexBuffer.at(k + s) = complexBuffer.at(k) - temp;
					complexBuffer.at(k) += temp;
				}
				b *= a;
			}
			a = Complex(sqrt((1.0 + a.real) / 2.0), isForward ? -sqrt((1.0 - a.real) * 0.5) : sqrt((1.0 - a.real) * 0.5));
		}
		if (!isForward)
		{
			for (size_t i = 0; i < nSample; i++)
			{
				complexBuffer.at(i) /= nSample;
			}
		}
	}
}