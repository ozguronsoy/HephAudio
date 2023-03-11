#pragma once
#include "framework.h"
#include "Complex.h"
#include "ComplexBuffer.h"
#include "FloatBuffer.h"

namespace HephAudio
{
	class Fourier final
	{
	public:
		static ComplexBuffer FFT_Forward(const FloatBuffer& floatBuffer);
		static ComplexBuffer FFT_Forward(const FloatBuffer& floatBuffer, size_t fftSize);
		static void FFT_Forward(ComplexBuffer& complexBuffer);
		static void FFT_Forward(ComplexBuffer& complexBuffer, size_t fftSize);
		static void FFT_Inverse(FloatBuffer& floatBuffer, ComplexBuffer& complexBuffer);
		static void FFT_Inverse(ComplexBuffer& complexBuffer, bool scale);
		static hephaudio_float BinFrequencyToIndex(size_t sampleRate, size_t fftSize, hephaudio_float frequency);
		static hephaudio_float IndexToBinFrequency(size_t sampleRate, size_t fftSize, size_t index);
		static size_t CalculateFFTSize(size_t bufferSize);
	private:
		static void ReverseBits(ComplexBuffer& complexBuffer, size_t fftSize);
		static void FFT(ComplexBuffer& complexBuffer, size_t fftSize, bool isForward);
	};
}