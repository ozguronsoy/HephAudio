#pragma once
#include "AudioBuffer.h"
#include "EchoInfo.h"
#include "EqualizerInfo.h"

typedef double (*FilterVolumeFunction)(double frequency);

namespace HephAudio
{
	class AudioProcessor final
	{
	private:
		static constexpr size_t defaultHopSize = 1024u;
		static constexpr size_t defaultFFTSize = 4096u;
#pragma region Converts, Mix, Split/Merge Channels
	public:
		static void ConvertBPS(AudioBuffer& buffer, uint16_t outputBps);
		static void ConvertChannels(AudioBuffer& buffer, uint16_t outputChannelCount);
		static void ConvertSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate);
		static void ConvertSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate, size_t outFrameCount);
		static void Mix(AudioBuffer& outputBuffer, AudioFormatInfo outputFormat, std::vector<AudioBuffer> inputBuffers);
		static std::vector<AudioBuffer> SplitChannels(const AudioBuffer& buffer);
		static AudioBuffer MergeChannels(const std::vector<AudioBuffer>& channels);
		static void ConvertPcmToInnerFormat(AudioBuffer& buffer);
		static void ConvertInnerToPcmFormat(AudioBuffer& buffer, size_t bps);
#pragma endregion
#pragma region Sound Effects
	public:
		static void Reverse(AudioBuffer& buffer);
		static void Echo(AudioBuffer& buffer, EchoInfo info);
		static void EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info);
		static void LinearPanning(AudioBuffer& buffer, double panningFactor);
		static void SquareLawPanning(AudioBuffer& buffer, double panningFactor);
		static void SineLawPanning(AudioBuffer& buffer, double panningFactor);
		static void SineWaveTremolo(AudioBuffer& buffer, double frequency, double depth, double phase);
		static void SineWaveTremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, double frequency, double depth, double phase);
		static void TriangleWaveTremolo(AudioBuffer& buffer, double frequency, double depth, double phase);
		static void TriangleWaveTremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, double frequency, double depth, double phase);
		static void Normalize(AudioBuffer& buffer, double peakAmplitude);
		static void RmsNormalize(AudioBuffer& buffer, double desiredRms);
		static void Equalizer(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos);
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double speed);
		static void PitchShift(AudioBuffer& buffer, double frequencyFactor);
		static void PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double frequencyFactor);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void HighPassFilter(AudioBuffer& buffer, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
#pragma endregion
#pragma region Windows
	public:
		static void ApplyTriangleWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateTriangleWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyParzenWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateParzenWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyWelchWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateWelchWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplySineWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateSineWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyHannWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateHannWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyHammingWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateHammingWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyBlackmanWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateBlackmanWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyExactBlackmanWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateExactBlackmanWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyNuttallWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateNuttallWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyBlackmanNuttallWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateBlackmanNuttallWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyBlackmanHarrisWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateBlackmanHarrisWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyFlatTopWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateFlatTopWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyGaussianWindow(AudioBuffer& buffer, double sigma);
		static AudioBuffer GenerateGaussianWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate, double sigma);
		static void ApplyTukeyWindow(AudioBuffer& buffer, double alpha);
		static AudioBuffer GenerateTukeyWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate, double alpha);
		static void ApplyBartlettHannWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateBartlettHannWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
		static void ApplyHannPoissonWindow(AudioBuffer& buffer, double alpha);
		static AudioBuffer GenerateHannPoissonWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate, double alpha);
		static void ApplyLanczosWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateLanczosWindow(size_t frameCount, uint16_t channelCount, uint32_t sampleRate);
#pragma endregion
#pragma region Processed Buffer
	private:
		struct ProcessedBuffer
		{
			size_t fStart;
			size_t hopSize;
			AudioBuffer audioBuffer;
			const AudioBuffer* pOriginalBuffer;
			ProcessedBuffer(const AudioBuffer* pOriginalBuffer, const AudioBuffer& audioBuffer, const size_t& fStart, const size_t& hopSize);
		};
	public:
		static ProcessedBuffer* GetProcessedBuffer(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart, const size_t& hopSize);
		static void RemoveOldProcessedBuffers(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart);
#pragma endregion
	};
}