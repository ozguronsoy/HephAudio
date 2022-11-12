#pragma once
#include "AudioBuffer.h"
#include "EchoInfo.h"
#include "EqualizerInfo.h"

#define TREMOLO_SINE_WAVE 0
#define TREMOLO_SQUARE_WAVE 1
#define TREMOLO_TRIANGLE_WAVE 2
#define TREMOLO_SAWTOOTH_WAVE 3

namespace HephAudio
{
	class AudioProcessor final
	{
	private:
		static constexpr size_t defaultHopSize = 4096u;
		static constexpr size_t defaultFFTSize = 8192u;
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
		// Adds echo to the given subBuffer in real-time using the originalBuffer, subBufferFrameIndex and the subBuffers frame count to calculate the echo.
		// Note that this method only adds the echo data to the given subBuffer, thus you should provide the subBuffer data from the originalBuffer.
		static void EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info);
		static void LinearPanning(AudioBuffer& buffer, double panningFactor);
		static void SquareLawPanning(AudioBuffer& buffer, double panningFactor);
		static void SineLawPanning(AudioBuffer& buffer, double panningFactor);
		static void Tremolo(AudioBuffer& buffer, double frequency, double depth, double phase, uint8_t waveType);
		static void TremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, double frequency, double depth, double phase, uint8_t waveType);
		static void Equalizer(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos);
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, double cutoffFreq, double transitionBandLength);
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, double transitionBandLength);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double cutoffFreq, double transitionBandLength);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double cutoffFreq, double transitionBandLength);
		static void HighPassFilter(AudioBuffer& buffer, double cutoffFreq, double transitionBandLength);
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, double transitionBandLength);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double cutoffFreq, double transitionBandLength);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double cutoffFreq, double transitionBandLength);
		static void BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
		static void BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, double transitionBandLength);
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
#pragma region Maximize Volume
	public:
		static double FindMaxVolume(const AudioBuffer& buffer);
		static void MaximizeVolume(AudioBuffer& buffer);
#pragma endregion
#pragma region Processed Buffer
	private:
		struct ProcessedBuffer
		{
			size_t fStart;
			AudioBuffer audioBuffer;
			const AudioBuffer* pOriginalBuffer;
			ProcessedBuffer();
			ProcessedBuffer(const AudioBuffer* pOriginalBuffer, const AudioBuffer& audioBuffer, size_t fStart);
		};
	public:
		static ProcessedBuffer* GetProcessedBuffer(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart);
		static void RemoveOldProcessedBuffers(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart);
#pragma endregion
	};
}