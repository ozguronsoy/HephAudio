#pragma once
#include "HephAudioFramework.h"
#include "../HephCommon/HeaderFiles/Complex.h"
#include "../HephCommon/HeaderFiles/ComplexBuffer.h"
#include "../HephCommon/HeaderFiles/FloatBuffer.h"

namespace HephAudio
{
	class Fourier final
	{
	public:
		static HephCommon::ComplexBuffer FFT_Forward(const HephCommon::FloatBuffer& floatBuffer);
		static HephCommon::ComplexBuffer FFT_Forward(const HephCommon::FloatBuffer& floatBuffer, size_t fftSize);
		static void FFT_Forward(HephCommon::ComplexBuffer& complexBuffer);
		static void FFT_Forward(HephCommon::ComplexBuffer& complexBuffer, size_t fftSize);
		static void FFT_Inverse(HephCommon::FloatBuffer& floatBuffer, HephCommon::ComplexBuffer& complexBuffer);
		static void FFT_Inverse(HephCommon::ComplexBuffer& complexBuffer, bool scale);
		static heph_float BinFrequencyToIndex(size_t sampleRate, size_t fftSize, heph_float frequency);
		static heph_float IndexToBinFrequency(size_t sampleRate, size_t fftSize, size_t index);
		static size_t CalculateFFTSize(size_t bufferSize);
	private:
		static void ReverseBits(HephCommon::ComplexBuffer& complexBuffer, size_t fftSize);
		static void FFT(HephCommon::ComplexBuffer& complexBuffer, size_t fftSize, bool isForward);
	};
}