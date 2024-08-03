#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "EchoInfo.h"
#include "EqualizerInfo.h"
#include "Oscillators/Oscillator.h"
#include "Windows/Window.h"
#include "FloatBuffer.h"
#include <vector>

namespace HephAudio
{
	class AudioProcessor final
	{
	private:
		static constexpr size_t DEFAULT_HOP_SIZE = 1024u;
		static constexpr size_t DEFAULT_FFT_SIZE = 4096u;
	public:
		AudioProcessor() = delete;
		AudioProcessor(const AudioProcessor&) = delete;
		AudioProcessor& operator=(const AudioProcessor&) = delete;
#pragma region Converts, Mix, Split/Merge Channels
	public:
		static void ChangeBitsPerSample(AudioBuffer& buffer, uint16_t outputBitsPerSample);
		static void ChangeChannelLayout(AudioBuffer& buffer, const AudioChannelLayout& outputChannelLayout);
		static void ChangeSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate);
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
		static void LinearPanning(AudioBuffer& buffer, double panningFactor);
		static void SquareLawPanning(AudioBuffer& buffer, double panningFactor);
		static void SineLawPanning(AudioBuffer& buffer, double panningFactor);
		static void Tremolo(AudioBuffer& buffer, double depth, const Oscillator& lfo);
		static void Vibrato(AudioBuffer& buffer, double depth, double extent_semitone, const Oscillator& lfo);
		static void Chorus(AudioBuffer& buffer, double depth, double feedbackGain, double baseDelay_ms, double delay_ms, double extent_semitone, const Oscillator& lfo);
		static void Flanger(AudioBuffer& buffer, double depth, double feedbackGain, double baseDelay_ms, double delay_ms, const Oscillator& lfo);
		static void FixOverflow(AudioBuffer& buffer);
		static void Normalize(AudioBuffer& buffer, heph_audio_sample_t peakAmplitude);
		static void RmsNormalize(AudioBuffer& buffer, double rms);
		static void HardClipDistortion(AudioBuffer& buffer, double clippingLevel_dB);
		static void ArctanDistortion(AudioBuffer& buffer, double alpha);
		static void CubicDistortion(AudioBuffer& buffer, double a);
		static void Overdrive(AudioBuffer& buffer, double drive);
		static void Fuzz(AudioBuffer& buffer, double depth, double alpha);
		static void LinearFadeIn(AudioBuffer& buffer, double duration_s);
		static void LinearFadeIn(AudioBuffer& buffer, double duration_s, size_t startIndex);
		static void LinearFadeOut(AudioBuffer& buffer, double duration_s);
		static void LinearFadeOut(AudioBuffer& buffer, double duration_s, size_t startIndex);
		static void Equalizer(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos);
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, size_t threadCountPerChannel, Window& window, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos);
		static void EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, size_t threadCountPerChannel, Window& window, const std::vector<EqualizerInfo>& infos);
		static void ChangeSpeed(AudioBuffer& buffer, double speed, Window& window);
		static void ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t windowSize, double speed, Window& window);
		static void PitchShift(AudioBuffer& buffer, double pitchChange_semitone, Window& window);
		static void PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, Window& window);
		static void PitchShiftMT(AudioBuffer& buffer, double pitchChange_semitone, Window& window);
		static void PitchShiftMT(AudioBuffer& buffer, double pitchChange_semitone, size_t threadCountPerChannel, Window& window);
		static void PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, Window& window);
		static void PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, size_t threadCountPerChannel, Window& window);
#pragma endregion
#pragma region Filters
	public:
		static void LowPassFilter(AudioBuffer& buffer, double cutoffFreq, Window& window);
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window);
		static void LowPassFilterMT(AudioBuffer& buffer, double cutoffFreq, Window& window);
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window);
		static void LowPassFilterMT(AudioBuffer& buffer, double cutoffFreq, size_t threadCountPerChannel, Window& window);
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, size_t threadCountPerChannel, Window& window);
		static void HighPassFilter(AudioBuffer& buffer, double cutoffFreq, Window& window);
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window);
		static void HighPassFilterMT(AudioBuffer& buffer, double cutoffFreq, Window& window);
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window);
		static void HighPassFilterMT(AudioBuffer& buffer, double cutoffFreq, size_t threadCountPerChannel, Window& window);
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, size_t threadCountPerChannel, Window& window);
		static void BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window);
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window);
		static void BandPassFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window);
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window);
		static void BandPassFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window);
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window);
		static void BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window);
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window);
		static void BandCutFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window);
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window);
		static void BandCutFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window);
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window);
#pragma endregion
	};
}