#pragma once
#include "framework.h"
#include "Complex.h"
#include "AudioBuffer.h"

namespace HephAudio
{
	class Fourier final
	{
	private:
		enum class FourierMethod : uint8_t
		{
			Null = 0,
			InverseTransform = 1,
			ForwardTransform = 2
		};
	private:
		uint32_t p; // fft size as power of two. (for p = 10, fft size = pow(2, 10) = 1024)
		FourierMethod lastMethod; // Last called method.
	public:
		ComplexBuffer complexBuffer;
	public:
		Fourier(const AudioBuffer& buffer);
		Fourier(const AudioBuffer& buffer, size_t fftSize);
		Fourier(const ComplexBuffer& complexBuffer);
		Fourier(const ComplexBuffer& complexBuffer, size_t fftSize);
		bool Forward();
		bool Inverse();
		void ComplexBufferToAudioBuffer(AudioBuffer& buffer) const;
		static double Magnitude(Complex sample);
		static double MagnitudeSquared(Complex sample);
		static double Phase(Complex sample, bool isDegree);
		static double Decibels(Complex sample);
		static double FrequencyToIndex(size_t sampleRate, size_t fftSize, double frequency);
		static size_t CalculateFFTSize(size_t bufferSize);
	private:
		void ReverseBits();
		void FFT(const bool isForward);
	};
}