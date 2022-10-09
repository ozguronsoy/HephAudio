#pragma once
#include "AudioBuffer.h"
#include "EchoInfo.h"

namespace HephAudio
{
	class AudioProcessor final
	{
#pragma region Converts, Mix, Split/Merge Channels
	public:
		// BPS = Bits Per Sample
		static void ConvertBPS(AudioBuffer& buffer, AudioFormatInfo outputFormat);
		// Mono to stereo, stereo to mono, from two channels to three channels...
		static void ConvertChannels(AudioBuffer& buffer, AudioFormatInfo outputFormat);
		static void ConvertSampleRate(AudioBuffer& buffer, AudioFormatInfo outputFormat);
		static void ConvertSampleRate(AudioBuffer& buffer, AudioFormatInfo outputFormat, size_t outFrameCount);
		static void Mix(AudioBuffer& outputBuffer, AudioFormatInfo outputFormat, std::vector<AudioBuffer> inputBuffers);
		static std::vector<AudioBuffer> SplitChannels(const AudioBuffer& buffer);
		static AudioBuffer MergeChannels(const std::vector<AudioBuffer>& channels);
#pragma endregion
#pragma region Sound Effects
	public:
		static void Reverse(AudioBuffer& buffer);
		// Adds echo to the given buffer.
		static void Echo(AudioBuffer& buffer, EchoInfo info);
		// Adds echo to the given subBuffer using the originalBuffer in real-time, subBufferFrameIndex and the subBuffers frame count to calculate the echo.
		// Note that this method only adds the echo data to the given subBuffer, thus you should provide the subBuffer data from the originalBuffer.
		static void EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info);
		static void Equalizer(AudioBuffer& buffer, double f1, double f2, double volume);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, double cutoffFreq, double transitionBandLength);
		static void HighPassFilter(AudioBuffer& buffer, double cutoffFreq, double transitionBandLength);
		static void BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
		static void BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
#pragma endregion
#pragma region Windows
	public:
		static void TriangleWindow(AudioBuffer& buffer);
		static void ParzenWindow(AudioBuffer& buffer);
		static void WelchWindow(AudioBuffer& buffer);
		static void SineWindow(AudioBuffer& buffer);
		static void HannWindow(AudioBuffer& buffer);
		static void HammingWindow(AudioBuffer& buffer);
		static void BlackmanWindow(AudioBuffer& buffer);
		static void ExactBlackmanWindow(AudioBuffer& buffer);
		static void NuttallWindow(AudioBuffer& buffer);
		static void BlackmanNuttallWindow(AudioBuffer& buffer);
		static void BlackmanHarrisWindow(AudioBuffer& buffer);
		static void FlatTopWindow(AudioBuffer& buffer);
		static void GaussianWindow(AudioBuffer& buffer, double sigma);
		static void TukeyWindow(AudioBuffer& buffer, double alpha);
		static void BartlettHannWindow(AudioBuffer& buffer);
		static void HannPoissonWindow(AudioBuffer& buffer, double alpha);
		static void LanczosWindow(AudioBuffer& buffer);
#pragma endregion
	};
}