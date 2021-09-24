#pragma once
#include "framework.h"
#include "Complex.h"
#include "AudioBuffer.h"

using namespace HephAudio::Structs;

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
		uint32_t p; // Number of samples as power of two. (for p = 10, number of samples = pow(2, 10) = 1024)
		FourierMethod lastMethod; // Last called method.
	public:
		ComplexBuffer complexBuffer;
	public:
		Fourier(const AudioBuffer& buffer);
		Fourier(const ComplexBuffer& complexBuffer);
		Fourier(const Fourier&) = delete;
		Fourier& operator=(const Fourier&) = delete;
		bool Forward();
		bool Inverse();
		static double Magnitude(Complex sample);
		static double MagnitudeSquared(Complex sample);
		static double Phase(Complex sample, bool isDegree);
		static double Decibels(Complex sample);
	private:
		void ReverseBits();
		void FFT(const bool isForward);
	};
}