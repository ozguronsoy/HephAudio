#pragma once
#include "AudioBuffer.h"
#include "EchoInfo.h"
#include "EqualizerInfo.h"

#define TREMOLO_SINE_WAVE 0
#define TREMOLO_SQUARE_WAVE 1
#define TREMOLO_TRIANGLE_WAVE 2
#define TREMOLO_SAWTOOTH_WAVE 3

typedef double (*FilterVolumeFunction)(double frequency);

namespace HephAudio
{
	class AudioProcessor final
	{
	private:
		static constexpr size_t defaultHopSize = 4096u;
		static constexpr size_t defaultFFTSize = 8192u;
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
		static void Tremolo(AudioBuffer& buffer, double frequency, double depth, double phase, uint8_t waveType);
		static void TremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, double frequency, double depth, double phase, uint8_t waveType);
		static void Normalize(AudioBuffer& buffer);
		static void Equalizer(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos);
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, const std::vector<EqualizerInfo>& infos);
		static void ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double speed);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void HighPassFilter(AudioBuffer& buffer, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double cutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, double lowCutoffFreq, double highCutoffFreq, FilterVolumeFunction volumeFunction);
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