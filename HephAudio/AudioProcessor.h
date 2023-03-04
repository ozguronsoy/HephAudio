#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "EchoInfo.h"
#include "EqualizerInfo.h"
#include "SineWaveOscillator.h"
#include "SquareWaveOscillator.h"
#include "TriangleWaveOscillator.h"
#include "SawToothWaveOscillator.h"

namespace HephAudio
{
	class AudioProcessor final
	{
	private:
		static constexpr size_t defaultHopSize = 1024u;
		static constexpr size_t defaultFFTSize = 4096u;
	public:
		AudioProcessor() = delete;
		AudioProcessor(const AudioProcessor&) = delete;
		AudioProcessor& operator=(const AudioProcessor&) = delete;
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
		static void ReverseRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex);
		static void Echo(AudioBuffer& buffer, EchoInfo info);
		static void EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info);
		static void LinearPanning(AudioBuffer& buffer, HEPHAUDIO_DOUBLE panningFactor);
		static void SquareLawPanning(AudioBuffer& buffer, HEPHAUDIO_DOUBLE panningFactor);
		static void SineLawPanning(AudioBuffer& buffer, HEPHAUDIO_DOUBLE panningFactor);
		static void Tremolo(AudioBuffer& buffer, HEPHAUDIO_DOUBLE depth, const OscillatorBase& lfo);
		static void TremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, HEPHAUDIO_DOUBLE depth, const OscillatorBase& lfo);
		static void Normalize(AudioBuffer& buffer, HEPHAUDIO_DOUBLE peakAmplitude);
		static void RmsNormalize(AudioBuffer& buffer, HEPHAUDIO_DOUBLE desiredRms);
		static void HardClipDistortion(AudioBuffer& buffer, HEPHAUDIO_DOUBLE clippingLevel_dB);
		static void SoftClipDistortion(AudioBuffer& buffer, HEPHAUDIO_DOUBLE alpha);
		static void Overdrive(AudioBuffer& buffer, HEPHAUDIO_DOUBLE drive);
		static void Fuzz(AudioBuffer& buffer, HEPHAUDIO_DOUBLE depth, HEPHAUDIO_DOUBLE alpha);
		static void Flanger(AudioBuffer& buffer, HEPHAUDIO_DOUBLE depth, HEPHAUDIO_DOUBLE delay_ms, const OscillatorBase& lfo);
		static void Equalizer(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos);
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE speed);
		static void PitchShift(AudioBuffer& buffer, HEPHAUDIO_DOUBLE shiftFactor);
		static void PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE shiftFactor);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, HEPHAUDIO_DOUBLE cutoffFreq);
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE cutoffFreq);
		static void LowPassFilterMT(AudioBuffer& buffer, HEPHAUDIO_DOUBLE cutoffFreq);
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE cutoffFreq);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, HEPHAUDIO_DOUBLE cutoffFreq);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE cutoffFreq);
		static void HighPassFilter(AudioBuffer& buffer, HEPHAUDIO_DOUBLE cutoffFreq);
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE cutoffFreq);
		static void HighPassFilterMT(AudioBuffer& buffer, HEPHAUDIO_DOUBLE cutoffFreq);
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE cutoffFreq);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, HEPHAUDIO_DOUBLE cutoffFreq);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE cutoffFreq);
		static void BandPassFilter(AudioBuffer& buffer, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandPassFilterMT(AudioBuffer& buffer, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandCutFilter(AudioBuffer& buffer, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandCutFilterMT(AudioBuffer& buffer, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, HEPHAUDIO_DOUBLE lowCutoffFreq, HEPHAUDIO_DOUBLE highCutoffFreq);
#pragma endregion
#pragma region Windows
	public:
		static void ApplyTriangleWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateTriangleWindow(size_t frameCount);
		static void ApplyParzenWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateParzenWindow(size_t frameCount);
		static void ApplyWelchWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateWelchWindow(size_t frameCount);
		static void ApplySineWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateSineWindow(size_t frameCount);
		static void ApplyHannWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateHannWindow(size_t frameCount);
		static void ApplyHammingWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateHammingWindow(size_t frameCount);
		static void ApplyBlackmanWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateBlackmanWindow(size_t frameCount);
		static void ApplyExactBlackmanWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateExactBlackmanWindow(size_t frameCount);
		static void ApplyNuttallWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateNuttallWindow(size_t frameCount);
		static void ApplyBlackmanNuttallWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateBlackmanNuttallWindow(size_t frameCount);
		static void ApplyBlackmanHarrisWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateBlackmanHarrisWindow(size_t frameCount);
		static void ApplyFlatTopWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateFlatTopWindow(size_t frameCount);
		static void ApplyGaussianWindow(AudioBuffer& buffer, HEPHAUDIO_DOUBLE sigma);
		static AudioBuffer GenerateGaussianWindow(size_t frameCount, HEPHAUDIO_DOUBLE sigma);
		static void ApplyTukeyWindow(AudioBuffer& buffer, HEPHAUDIO_DOUBLE alpha);
		static AudioBuffer GenerateTukeyWindow(size_t frameCount, HEPHAUDIO_DOUBLE alpha);
		static void ApplyBartlettHannWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateBartlettHannWindow(size_t frameCount);
		static void ApplyHannPoissonWindow(AudioBuffer& buffer, HEPHAUDIO_DOUBLE alpha);
		static AudioBuffer GenerateHannPoissonWindow(size_t frameCount, HEPHAUDIO_DOUBLE alpha);
		static void ApplyLanczosWindow(AudioBuffer& buffer);
		static AudioBuffer GenerateLanczosWindow(size_t frameCount);
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