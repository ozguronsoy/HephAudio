#include "Fourier.h"
#include "AudioException.h"

namespace HephAudio
{
	Fourier::Fourier(const AudioBuffer& buffer) : Fourier(buffer, 0u) {}
	Fourier::Fourier(const AudioBuffer& buffer, size_t fftSize)
	{
		lastMethod = FourierMethod::Null;
		if (buffer.GetFormat().channelCount != 1)
		{
			throw AudioException(E_INVALIDARG, L"Fourier::Fourier", L"Channel count of the audio buffer must be 1.");
		}
		if (fftSize == 0)
		{
			fftSize = buffer.FrameCount();
		}
		fftSize = CalculateFFTSize(fftSize);
		p = log2f(fftSize);
		complexBuffer.resize(fftSize, Complex());
		for (size_t i = 0; i < buffer.FrameCount(); i++)
		{
			complexBuffer.at(i).real = buffer.Get(i, 0);
		}
	}
	Fourier::Fourier(const ComplexBuffer& bufferToCopy) : Fourier(bufferToCopy, 0u) {}
	Fourier::Fourier(const ComplexBuffer& bufferToCopy, size_t fftSize)
	{
		lastMethod = FourierMethod::Null;
		if (fftSize == 0)
		{
			fftSize = bufferToCopy.size();
		}
		fftSize = CalculateFFTSize(fftSize);
		p = log2f(fftSize);
		complexBuffer.resize(fftSize, Complex());
		memcpy(&complexBuffer.at(0), &bufferToCopy.at(0), bufferToCopy.size() * sizeof(Complex));
	}
	bool Fourier::Forward()
	{
		if (lastMethod != FourierMethod::ForwardTransform)
		{
			FFT(true);
			lastMethod = FourierMethod::ForwardTransform;
			return true;
		}
		return false;
	}
	bool Fourier::Inverse()
	{
		if (lastMethod != FourierMethod::InverseTransform)
		{
			FFT(false);
			lastMethod = FourierMethod::InverseTransform;
			return true;
		}
		return false;
	}
	void Fourier::ComplexBufferToAudioBuffer(AudioBuffer& buffer) const
	{
		const size_t bufferSize = complexBuffer.size() > buffer.FrameCount() ? buffer.FrameCount() : complexBuffer.size();
		for (size_t i = 0; i < bufferSize; i++)
		{
			buffer.Set(complexBuffer.at(i).real, i, 0);
		}
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
			return pow(2, ceil(log2f(bufferSize))); // smallest power of 2 thats greater than nSample
		}
		return bufferSize;
	}
	void Fourier::ReverseBits()
	{
		size_t k;
		ComplexBuffer resultBuffer(complexBuffer.size(), Complex());
		for (size_t i = 0; i < complexBuffer.size(); i++)
		{
			k = 0u;
			for (size_t j = 0; j < p; j++)
			{
				const size_t jp = pow(2, j);
				if ((i & jp) == jp)
				{
					k += pow(2, p - j - 1);
				}
				resultBuffer.at(i) = complexBuffer.at(k);
			}
		}
		complexBuffer = resultBuffer;
	}
	void Fourier::FFT(const bool isForward)
	{
		ReverseBits();
		const size_t nSample = complexBuffer.size();
		Complex a = Complex(-1.0, 0.0);
		for (size_t i = 0; i < p; i++)
		{
			const size_t s = pow(2, i);
			const size_t s2 = s * 2;
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