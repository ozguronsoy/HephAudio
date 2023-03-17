#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "FloatBuffer.h"
#include "EchoInfo.h"
#include "EqualizerInfo.h"
#include "SineWaveOscillator.h"
#include "SquareWaveOscillator.h"
#include "TriangleWaveOscillator.h"
#include "SawToothWaveOscillator.h"
#include "PulseWaveOscillator.h"

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
		static void ConvertSampleRateRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, uint32_t outputSampleRate, size_t outFrameCount);
		static void Mix(AudioBuffer& outputBuffer, AudioFormatInfo outputFormat, std::vector<AudioBuffer> inputBuffers);
		static std::vector<FloatBuffer> SplitChannels(const AudioBuffer& buffer);
		static AudioBuffer MergeChannels(const std::vector<FloatBuffer>& channels, uint32_t sampleRate);
		static void ConvertPcmToInnerFormat(AudioBuffer& buffer, Endian pcmEndian);
		static AudioBuffer ConvertPcmToInnerFormat(void* buffer, size_t frameCount, AudioFormatInfo formatInfo, Endian pcmEndian);
		static void ConvertInnerToPcmFormat(AudioBuffer& buffer, size_t bps, Endian pcmEndian);
#pragma endregion
#pragma region Sound Effects
	public:
		static void Reverse(AudioBuffer& buffer);
		static void ReverseRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex);
		static void Echo(AudioBuffer& buffer, EchoInfo info);
		static void EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info);
		static void LinearPanning(AudioBuffer& buffer, hephaudio_float panningFactor);
		static void SquareLawPanning(AudioBuffer& buffer, hephaudio_float panningFactor);
		static void SineLawPanning(AudioBuffer& buffer, hephaudio_float panningFactor);
		static void Tremolo(AudioBuffer& buffer, hephaudio_float depth, const Oscillator& lfo);
		static void TremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float depth, const Oscillator& lfo);
		static void Vibrato(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float extent_semitone, const Oscillator& lfo);
		static void VibratoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float depth, hephaudio_float extent_semitone, const Oscillator& lfo);
		static void Normalize(AudioBuffer& buffer, hephaudio_float peakAmplitude);
		static void RmsNormalize(AudioBuffer& buffer, hephaudio_float desiredRms);
		static void HardClipDistortion(AudioBuffer& buffer, hephaudio_float clippingLevel_dB);
		static void SoftClipDistortion(AudioBuffer& buffer, hephaudio_float alpha);
		static void Overdrive(AudioBuffer& buffer, hephaudio_float drive);
		static void Fuzz(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float alpha);
		static void Flanger(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float feedbackGain, hephaudio_float baseDelay_ms, hephaudio_float delay_ms, const Oscillator& lfo);
		static void FlangerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float depth, hephaudio_float feedbackGain, hephaudio_float baseDelay_ms, hephaudio_float delay_ms, const Oscillator& lfo);
		static void Wah(AudioBuffer& buffer, hephaudio_float depth, hephaudio_float damping, hephaudio_float fcmin, hephaudio_float fcmax, const Oscillator& lfo);
		static void Equalizer(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos);
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float speed);
		static void ChangeSpeedTD(AudioBuffer& buffer, size_t hopSize, size_t windowSize, hephaudio_float speed);
		static void PitchShift(AudioBuffer& buffer, hephaudio_float shiftFactor);
		static void PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float shiftFactor);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, hephaudio_float cutoffFreq);
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq);
		static void LowPassFilterMT(AudioBuffer& buffer, hephaudio_float cutoffFreq);
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float cutoffFreq);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq);
		static void HighPassFilter(AudioBuffer& buffer, hephaudio_float cutoffFreq);
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq);
		static void HighPassFilterMT(AudioBuffer& buffer, hephaudio_float cutoffFreq);
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float cutoffFreq);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, hephaudio_float cutoffFreq);
		static void BandPassFilter(AudioBuffer& buffer, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandPassFilterMT(AudioBuffer& buffer, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandCutFilter(AudioBuffer& buffer, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandCutFilterMT(AudioBuffer& buffer, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, hephaudio_float lowCutoffFreq, hephaudio_float highCutoffFreq);
#pragma endregion
#pragma region Windows
	public:
		static void ApplyTriangleWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateTriangleWindow(size_t frameCount);
		static void ApplyParzenWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateParzenWindow(size_t frameCount);
		static void ApplyWelchWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateWelchWindow(size_t frameCount);
		static void ApplySineWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateSineWindow(size_t frameCount);
		static void ApplyHannWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateHannWindow(size_t frameCount);
		static void ApplyHammingWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateHammingWindow(size_t frameCount);
		static void ApplyBlackmanWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateBlackmanWindow(size_t frameCount);
		static void ApplyExactBlackmanWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateExactBlackmanWindow(size_t frameCount);
		static void ApplyNuttallWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateNuttallWindow(size_t frameCount);
		static void ApplyBlackmanNuttallWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateBlackmanNuttallWindow(size_t frameCount);
		static void ApplyBlackmanHarrisWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateBlackmanHarrisWindow(size_t frameCount);
		static void ApplyFlatTopWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateFlatTopWindow(size_t frameCount);
		static void ApplyGaussianWindow(AudioBuffer& buffer, hephaudio_float sigma);
		static FloatBuffer GenerateGaussianWindow(size_t frameCount, hephaudio_float sigma);
		static void ApplyTukeyWindow(AudioBuffer& buffer, hephaudio_float alpha);
		static FloatBuffer GenerateTukeyWindow(size_t frameCount, hephaudio_float alpha);
		static void ApplyBartlettHannWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateBartlettHannWindow(size_t frameCount);
		static void ApplyHannPoissonWindow(AudioBuffer& buffer, hephaudio_float alpha);
		static FloatBuffer GenerateHannPoissonWindow(size_t frameCount, hephaudio_float alpha);
		static void ApplyLanczosWindow(AudioBuffer& buffer);
		static FloatBuffer GenerateLanczosWindow(size_t frameCount);
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
		static ProcessedBuffer* GetProcessedBuffer(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart, const size_t& hopSize);
		static void RemoveOldProcessedBuffers(std::vector<ProcessedBuffer*>& processedBuffers, const AudioBuffer* const pOriginalBuffer, const size_t& fStart);
#pragma endregion
	};
}