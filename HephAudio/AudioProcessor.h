#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "FloatBuffer.h"
#include "EchoInfo.h"
#include "EqualizerInfo.h"
#include "Oscillator.h"
#include "Window.h"
#include <vector>

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
		static void ConvertSampleRate(const AudioBuffer& originalBuffer, AudioBuffer& subBuffer, size_t subBufferFrameIndex, uint32_t outputSampleRate, size_t outFrameCount);
		static void Mix(AudioBuffer& outputBuffer, AudioFormatInfo outputFormat, std::vector<AudioBuffer> inputBuffers);
		static std::vector<HephCommon::FloatBuffer> SplitChannels(const AudioBuffer& buffer);
		static AudioBuffer MergeChannels(const std::vector<HephCommon::FloatBuffer>& channels, uint32_t sampleRate);
		static void ConvertToInnerFormat(AudioBuffer& buffer);
		static void ConvertToTargetFormat(AudioBuffer& buffer, AudioFormatInfo targetFormat);
		static void ChangeEndian(AudioBuffer& buffer);
#pragma endregion
#pragma region Sound Effects
	public:
		static void Reverse(AudioBuffer& buffer);
		static void Echo(AudioBuffer& buffer, EchoInfo info);
		static void LinearPanning(AudioBuffer& buffer, heph_float panningFactor);
		static void SquareLawPanning(AudioBuffer& buffer, heph_float panningFactor);
		static void SineLawPanning(AudioBuffer& buffer, heph_float panningFactor);
		static void Tremolo(AudioBuffer& buffer, heph_float depth, const Oscillator& lfo);
		static void Vibrato(AudioBuffer& buffer, heph_float depth, heph_float extent_semitone, const Oscillator& lfo);
		static void Chorus(AudioBuffer& buffer, heph_float depth, heph_float feedbackGain, heph_float baseDelay_ms, heph_float delay_ms, heph_float extent_semitone, const Oscillator& lfo);
		static void Flanger(AudioBuffer& buffer, heph_float depth, heph_float feedbackGain, heph_float baseDelay_ms, heph_float delay_ms, const Oscillator& lfo);
		static void Wah(AudioBuffer& buffer, heph_float depth, heph_float damping, heph_float fcmin, heph_float fcmax, const Oscillator& lfo);
		static void Normalize(AudioBuffer& buffer, heph_float peakAmplitude);
		static void RmsNormalize(AudioBuffer& buffer, heph_float desiredRms);
		static void HardClipDistortion(AudioBuffer& buffer, heph_float clippingLevel_dB);
		static void SoftClipDistortion(AudioBuffer& buffer, heph_float alpha);
		static void Overdrive(AudioBuffer& buffer, heph_float drive);
		static void Fuzz(AudioBuffer& buffer, heph_float depth, heph_float alpha);
		static void Equalizer(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos);
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos);
		static void ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float speed, Window& window);
		static void ChangeSpeedTD(AudioBuffer& buffer, heph_float speed, Window& window);
		static void ChangeSpeedTD(AudioBuffer& buffer, size_t hopSize, size_t windowSize, heph_float speed, Window& window);
		static void PitchShift(AudioBuffer& buffer, heph_float pitchChange_semitone, Window& window);
		static void PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone, Window& window);
		static void PitchShiftMT(AudioBuffer& buffer, heph_float pitchChange_semitone, Window& window);
		static void PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float pitchChange_semitone, Window& window);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, heph_float cutoffFreq, Window& window);
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window);
		static void LowPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, Window& window);
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window);
		static void LowPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window);
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window);
		static void HighPassFilter(AudioBuffer& buffer, heph_float cutoffFreq, Window& window);
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window);
		static void HighPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, Window& window);
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, Window& window);
		static void HighPassFilterMT(AudioBuffer& buffer, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window);
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float cutoffFreq, size_t threadCountPerChannel, Window& window);
		static void BandPassFilter(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window);
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window);
		static void BandPassFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window);
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window);
		static void BandPassFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window);
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window);
		static void BandCutFilter(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window);
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window);
		static void BandCutFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window);
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, Window& window);
		static void BandCutFilterMT(AudioBuffer& buffer, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window);
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, heph_float lowCutoffFreq, heph_float highCutoffFreq, size_t threadCountPerChannel, Window& window);
		static void SchroederAllpassFilter(AudioBuffer& buffer, heph_float delay_ms, heph_float gain);
		static void FeedbackCombFilter(AudioBuffer& buffer, heph_float delay_ms, heph_float gain);
#pragma endregion
	};
}