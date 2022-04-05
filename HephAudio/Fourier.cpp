#include "Fourier.h"
#include "AudioException.h"

namespace HephAudio
{
	Fourier::Fourier(const AudioBuffer& buffer, size_t fftSize)
	{
		lastMethod = FourierMethod::Null;
		if (buffer.GetFormat().nChannels != 1)
		{
			throw AudioException(E_INVALIDARG, L"Fourier::Fourier", L"Channel count of the buffer must be 1.");
		}
		if (fftSize == 0)
		{
			fftSize = buffer.FrameCount();
		}
		if (!(fftSize > 0 && !(fftSize & (fftSize - 1)))) // if not power of 2
		{
			p = log2f(fftSize) + 1;
			fftSize = pow(2, p); // smallest power of 2 thats greater than nSample
		}
		else
		{
			p = log2f(fftSize);
		}
		complexBuffer.resize(fftSize, Complex());
		for (size_t i = 0; i < buffer.FrameCount(); i++)
		{
			complexBuffer.at(i).real = buffer.Get(i, 0);
		}
	}
	Fourier::Fourier(const ComplexBuffer& bufferToCopy)
	{
		lastMethod = FourierMethod::Null;
		size_t fftSize = bufferToCopy.size();
		if (!(fftSize > 0 && !(fftSize & (fftSize - 1)))) // if not power of 2
		{
			p = log2f(fftSize) + 1;
			fftSize = pow(2, p); // smallest power of 2 thats greater than nSample
		}
		else
		{
			p = log2f(fftSize);
		}
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
		for (size_t i = 0; i < complexBuffer.size(); i++)
		{
			if (i >= buffer.FrameCount())
			{
				break;
			}
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
	void Fourier::ReverseBits()
	{
		uint32_t k = 0u;
		ComplexBuffer resultBuffer(complexBuffer.size(), Complex());
		for (size_t i = 0; i < complexBuffer.size(); i++)
		{
			k = 0u;
			for (size_t j = 0; j < p; j++)
			{
				const uint32_t jp = pow(2, j);
				if ((i & jp) == jp)
				{
					k += pow(2, p - 1 - j);
				}
				resultBuffer.at(i) = complexBuffer.at(k);
			}
		}
		complexBuffer = resultBuffer;
	}
	void Fourier::FFT(const bool isForward)
	{
		ReverseBits();
		/* Compute the FFT */
		const size_t nSample = complexBuffer.size();
		long i, i1, j, l, l1, l2;
		double c1, c2, t1, t2, u1, u2, z;
		c1 = -1.0;
		c2 = 0.0;
		l2 = 1;
		for (l = 0; l < p; l++)
		{
			l1 = l2;
			l2 <<= 1;
			u1 = 1.0;
			u2 = 0.0;
			for (j = 0; j < l1; j++)
			{
				for (i = j; i < nSample; i += l2)
				{
					i1 = i + l1;
					t1 = u1 * complexBuffer.at(i1).real - u2 * complexBuffer.at(i1).imaginary;
					t2 = u1 * complexBuffer.at(i1).imaginary + u2 * complexBuffer.at(i1).real;
					complexBuffer.at(i1).real = complexBuffer.at(i).real - t1;
					complexBuffer.at(i1).imaginary = complexBuffer.at(i).imaginary - t2;
					complexBuffer.at(i).real += t1;
					complexBuffer.at(i).imaginary += t2;
				}
				z = u1 * c1 - u2 * c2;
				u2 = u1 * c2 + u2 * c1;
				u1 = z;
			}
			c2 = sqrt((1.0 - c1) / 2.0);
			if (isForward)
			{
				c2 = -c2;
			}
			c1 = sqrt((1.0 + c1) / 2.0);
		}
		/* Scaling for inverse transform */
		if (!isForward)
		{
			for (i = 0; i < nSample; i++)
			{
				complexBuffer.at(i).real /= nSample;
				complexBuffer.at(i).imaginary /= nSample;
			}
		}
	}
}