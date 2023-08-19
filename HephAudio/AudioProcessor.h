#pragma once
#include "HephAudioFramework.h"
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
		static std::vector<HephCommon::FloatBuffer> SplitChannels(const AudioBuffer& buffer);
		static AudioBuffer MergeChannels(const std::vector<HephCommon::FloatBuffer>& channels, uint32_t sampleRate);
		static void ConvertToInnerFormat(AudioBuffer& buffer, Endian endian);
		static void ConvertToTargetFormat(AudioBuffer& buffer, AudioFormatInfo targetFormat, Endian endian);
		static void ChangeEndian(AudioBuffer& buffer);
#pragma endregion
#pragma region Sound Effects
	public:
		static void Reverse(AudioBuffer& buffer);
		static void ReverseRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex);
		static void Echo(AudioBuffer& buffer, EchoInfo info);
		static void EchoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, EchoInfo info);
		static void LinearPanning(AudioBuffer& buffer, heph_float panningFactor);
		static void SquareLawPanning(AudioBuffer& buffer, heph_float panningFactor);
		static void SineLawPanning(AudioBuffer& buffer, heph_float panningFactor);
		static void Tremolo(AudioBuffer& buffer, heph_float depth, const Oscillator& lfo);
		static void TremoloRT(AudioBuffer& subBuffer, size_t subBufferFrameIndex, heph_float depth, const Oscillator& lfo);
		static void Vibrato(AudioBuffer& buffer, heph_float depth, heph_float extent_semitone, const Oscillator& lfo);
		static void VibratoRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, heph_float depth, heph_float extent_semitone, const Oscillator& lfo);
		static void Chorus(AudioBuffer& buffer, heph_float depth, heph_float feedbackGain, heph_float baseDelay_ms, heph_float delay_ms, heph_float extent_semitone, const Oscillator& lfo);
		static void Flanger(AudioBuffer& buffer, heph_float depth, heph_float feedbackGain, heph_float baseDelay_ms, heph_float delay_ms, const Oscillator& lfo);
		static void FlangerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, heph_float depth, heph_float feedbackGain, heph_float baseDelay_ms, heph_float delay_ms, const Oscillator& lfo);
		static void Wah(AudioBuffer& buffer, heph_float depth, heph_float damping, heph_float fcmin, heph_float fcmax, const Oscillator& lfo);
		static void Normalize(AudioBuffer& buffer, heph_float peakAmplitude);
		static void RmsNormalize(AudioBuffer& buffer, heph_float desiredRms);
		static void HardClipDistortion(AudioBuffer& buffer, heph_float clippingLevel_dB);
		static void SoftClipDistortion(AudioBuffer& buffer, heph_float alpha);
		static void Overdrive(AudioBuffer& buffer, heph_float drive);
		static void Fuzz(AudioBuffer& buffer, heph_float depth, heph_float alpha);
		static void Equalizer(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos);
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, const std::vector<EqualizerInfo>& infos);
		static void EqualizerRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, const std::vector<EqualizerInfo>& infos);
		static void ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float speed);
		static void ChangeSpeedTD(AudioBuffer& buffer, heph_float speed);
		static void ChangeSpeedTD(AudioBuffer& buffer, size_t hopSize, size_t windowSize, heph_float speed);
		static void PitchShift(AudioBuffer& buffer, heph_float pitchChange_semitone);
		static void PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone);
		static void PitchShiftMT(AudioBuffer& buffer, heph_float pitchChange_semitone);
		static void PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, heph_float cutoffFreq);
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq);
		static void LowPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq);
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq);
		static void LowPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, size_t threadCountPerChannel);
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, size_t threadCountPerChannel);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, heph_float cutoffFreq);
		static void LowPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, heph_float cutoffFreq);
		static void HighPassFilter(AudioBuffer& buffer, heph_float cutoffFreq);
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq);
		static void HighPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq);
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq);
		static void HighPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, size_t threadCountPerChannel);
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, size_t threadCountPerChannel);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, heph_float cutoffFreq);
		static void HighPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, heph_float cutoffFreq);
		static void BandPassFilter(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandPassFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandPassFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel);
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandPassFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandCutFilter(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandCutFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandCutFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel);
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void BandCutFilterRT(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq);
		static void SchroederAllpassFilter(AudioBuffer& buffer, heph_float delay_ms, heph_float gain);
		static void FeedbackCombFilter(AudioBuffer& buffer, heph_float delay_ms, heph_float gain);
#pragma endregion
#pragma region Windows
	public:
		static void ApplyTriangleWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateTriangleWindow(size_t frameCount);
		static void ApplyParzenWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateParzenWindow(size_t frameCount);
		static void ApplyWelchWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateWelchWindow(size_t frameCount);
		static void ApplySineWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateSineWindow(size_t frameCount);
		static void ApplyHannWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateHannWindow(size_t frameCount);
		static void ApplyHammingWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateHammingWindow(size_t frameCount);
		static void ApplyBlackmanWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateBlackmanWindow(size_t frameCount);
		static void ApplyExactBlackmanWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateExactBlackmanWindow(size_t frameCount);
		static void ApplyNuttallWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateNuttallWindow(size_t frameCount);
		static void ApplyBlackmanNuttallWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateBlackmanNuttallWindow(size_t frameCount);
		static void ApplyBlackmanHarrisWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateBlackmanHarrisWindow(size_t frameCount);
		static void ApplyFlatTopWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateFlatTopWindow(size_t frameCount);
		static void ApplyGaussianWindow(AudioBuffer& buffer, heph_float sigma);
		static HephCommon::FloatBuffer GenerateGaussianWindow(size_t frameCount, heph_float sigma);
		static void ApplyTukeyWindow(AudioBuffer& buffer, heph_float alpha);
		static HephCommon::FloatBuffer GenerateTukeyWindow(size_t frameCount, heph_float alpha);
		static void ApplyBartlettHannWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateBartlettHannWindow(size_t frameCount);
		static void ApplyHannPoissonWindow(AudioBuffer& buffer, heph_float alpha);
		static HephCommon::FloatBuffer GenerateHannPoissonWindow(size_t frameCount, heph_float alpha);
		static void ApplyLanczosWindow(AudioBuffer& buffer);
		static HephCommon::FloatBuffer GenerateLanczosWindow(size_t frameCount);
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