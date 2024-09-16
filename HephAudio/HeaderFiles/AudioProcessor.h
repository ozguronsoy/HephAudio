#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "EchoInfo.h"
#include "EqualizerInfo.h"
#include "Oscillators/Oscillator.h"
#include "Windows/Window.h"
#include "Buffers/DoubleBuffer.h"
#include "Buffers/ComplexBuffer.h"
#include <vector>

/** @file */

namespace HephAudio
{
	/**
	 * @brief provides sound effects and filters.
	 * 
	 */
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
		/**
		 * changes the channel layout.
		 * 
		 */
		static void ChangeChannelLayout(AudioBuffer& buffer, const AudioChannelLayout& outputChannelLayout);

		/**
		 * changes the sample rate.
		 * 
		 */
		static void ChangeSampleRate(AudioBuffer& buffer, uint32_t outputSampleRate);

		/**
		 * splits the audio channels into separate buffers.
		 * 
		 */
		static std::vector<HephCommon::DoubleBuffer> SplitChannels(const AudioBuffer& buffer);

		/**
		 * merges the channels.
		 * 
		 */
		static AudioBuffer MergeChannels(const std::vector<HephCommon::DoubleBuffer>& channels, uint32_t sampleRate);

		/**
		 * changes the endianness of the audio samples.<br>
		 * Example: <b>s32le</b> -> <b>s32be</b>.
		 * 
		 */
		static void ChangeEndian(AudioBuffer& buffer);
#pragma endregion
#pragma region Sound Effects
	public:
		/**
		 * adds echo to the audio data.
		 * 
		 * @param info contains the echo characteristics.
		 */
		static void Echo(AudioBuffer& buffer, EchoInfo info);

		/**
		 * linearly increases the volume of one channel while decreasing the other one (stereo only).
		 * 
		 * @param panningFactor determines how the channels' volume will be changed. 
		 * Ranges between -1 and 1.<br>
		 * -1 means only the left channel will be audible and 1 means only the right channel will be audible.
		 * 
		 */
		static void LinearPanning(AudioBuffer& buffer, double panningFactor);

		/**
		 * increases the volume of one channel while decreasing the other one (stereo only). 
		 * Uses square root to calculate the volumes.
		 * 
		 * @param panningFactor determines how the channels' volume will be changed. 
		 * Ranges between -1 and 1.<br>
		 * -1 means only the left channel will be audible and 1 means only the right channel will be audible.
		 */
		static void SquareLawPanning(AudioBuffer& buffer, double panningFactor);

		/**
		 * increases the volume of one channel while decreasing the other one (stereo only). 
		 * Uses sin function to calculate the volumes.
		 * 
		 * @param panningFactor determines how the channels' volume will be changed. 
		 * Ranges between -1 and 1.<br>
		 * -1 means only the left channel will be audible and 1 means only the right channel will be audible.
		 */
		static void SineLawPanning(AudioBuffer& buffer, double panningFactor);

		/**
		 * changes the volume of the audio data periodically and mixes the result with the input signal.
		 * 
		 * @param depth controls the dry/wet mix ratio.
		 * Ranges between 0 and 1.
		 * 
		 * @param lfo low-frequency \link HephAudio::Oscillator oscillator \endlink.
		 */
		static void Tremolo(AudioBuffer& buffer, double depth, const Oscillator& lfo);

		/**
		 * changes the pitch of the audio data periodically and mixes the result with the input signal.
		 * 
		 * @param depth controls the dry/wet mix ratio.
		 * Ranges between 0 and 1.
		 * 
		 * @param extent_semitone maximum pitch change in terms of semitones.
		 * @param lfo low-frequency \link HephAudio::Oscillator oscillator \endlink.
		 */
		static void Vibrato(AudioBuffer& buffer, double depth, double extent_semitone, const Oscillator& lfo);

		/**
		 * delays the audio data and changes its pitch periodically. 
		 * Then mixes the result with the input signal.
		 * 
		 * @param depth controls the dry/wet mix ratio.
		 * Ranges between 0 and 1.
		 * 
		 * @param feedbackGain gain of the feedback sample.
		 * @param baseDelay_ms constant delay, in milliseconds, that will be added to the wet signal.
		 * @param delay_ms maximum value of the variable delay, in milliseconds, that will be added to the wet signal. 
		 * The delay applied to wet samples changes periodically.<br>
		 * The maximum delay that will be applied can be calculated as <b>baseDelay_ms + delay_ms</b>.
		 * 
		 * @param extent_semitone maximum pitch change in terms of semitones.
		 * @param lfo low-frequency \link HephAudio::Oscillator oscillator \endlink.
		 */
		static void Chorus(AudioBuffer& buffer, double depth, double feedbackGain, double baseDelay_ms, double delay_ms, double extent_semitone, const Oscillator& lfo);

		/**
		 * delays the audio data and mixes the result with the input signal. 
		 * The amount of delay applied changes periodically.
		 * 
		 * @param depth controls the dry/wet mix ratio.
		 * Ranges between 0 and 1.
		 * 
		 * @param feedbackGain gain of the feedback sample.
		 * @param baseDelay_ms constant delay, in milliseconds, that will be added to the wet signal.
		 * @param delay_ms maximum value of the variable delay, in milliseconds, that will be added to the wet signal. 
		 * The delay applied to wet samples changes periodically.<br>
		 * The maximum delay that will be applied can be calculated as <b>baseDelay_ms + delay_ms</b>.
		 * 
		 * @param lfo low-frequency \link HephAudio::Oscillator oscillator \endlink.
		 */
		static void Flanger(AudioBuffer& buffer, double depth, double feedbackGain, double baseDelay_ms, double delay_ms, const Oscillator& lfo);

		/**
		 * fixes the distortion due to floating point samples exceeding the [-1, 1] range.
		 * 
		 * @important this method does \a <b>NOT</b> work with the integer types.
		 */
		static void FixOverflow(AudioBuffer& buffer);

		/**
		 * normalizes the audio data at the provided amplitude.
		 * 
		 * @param peakAmplitude maximum amplitude of the output signal.
		 */
		static void Normalize(AudioBuffer& buffer, heph_audio_sample_t peakAmplitude);

		/**
		 * normalizes the audio data at the provided rms.
		 * 
		 * @param rms root mean square value of the output signal.
		 */
		static void RmsNormalize(AudioBuffer& buffer, double rms);

		/**
		 * applies hard-clipping distortion.
		 * 
		 * @param clippingLevel_dB threshold value in decibels. 
		 * Must be negative or zero.
		 */
		static void HardClipDistortion(AudioBuffer& buffer, double clippingLevel_dB);

		/**
		 * applies soft-clipping distortion via arctan function.
		 * 
		 * @param alpha controls the amount of distortion. 
		 * ranges between 0 and 1.
		 */
		static void ArctanDistortion(AudioBuffer& buffer, double alpha);

		/**
		 * applies cubic distortion.
		 * 
		 * @param a controls the amount of distortion. 
		 * ranges between 0 and 1.
		 */
		static void CubicDistortion(AudioBuffer& buffer, double a);

		/**
		 * applies overdrive distortion.
		 * 
		 * @param drive controls the amount of distortion. 
		 * ranges between 0 and 1.
		 */
		static void Overdrive(AudioBuffer& buffer, double drive);

		/**
		 * applies fuzz distortion.
		 * 
		 * @param depth controls the dry/wet mix ratio.
		 * Ranges between 0 and 1.
		 * 
		 * @param alpha controls the amount of distortion. 
		 * ranges between 0 and 11.
		 */
		static void Fuzz(AudioBuffer& buffer, double depth, double alpha);

		/**
		 * applies linear fade-in.
		 * 
		 * @param duration_s duration of the fade-in in seconds.
		 */
		static void LinearFadeIn(AudioBuffer& buffer, double duration_s);

		/**
		 * applies linear fade-in.
		 * 
		 * @param duration_s duration of the fade-in in seconds.
		 * @param startIndex index of the first audio frame the effect will be applied to.
		 */
		static void LinearFadeIn(AudioBuffer& buffer, double duration_s, size_t startIndex);

		/**
		 * applies linear fade-out.
		 * 
		 * @param duration_s duration of the fade-out in seconds.
		 */
		static void LinearFadeOut(AudioBuffer& buffer, double duration_s);

		/**
		 * applies linear fade-out.
		 * 
		 * @param duration_s duration of the fade-out in seconds.
		 * @param startIndex index of the first audio frame the effect will be applied to.
		 */
		static void LinearFadeOut(AudioBuffer& buffer, double duration_s, size_t startIndex);

		/**
		 * equalizer.
		 * 
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 * @param infos contains the equalizer characteristics.
		 */
		static void Equalizer(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos);

		/**
		 * equalizer.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 * @param infos contains the equalizer characteristics.
		 */
		static void Equalizer(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos);

		/**
		 * multithreaded equalizer.
		 * 
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 * @param infos contains the equalizer characteristics.
		 */
		static void EqualizerMT(AudioBuffer& buffer, Window& window, const std::vector<EqualizerInfo>& infos);

		/**
		 * multithreaded equalizer.
		 * 
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 * @param infos contains the equalizer characteristics.
		 */
		static void EqualizerMT(AudioBuffer& buffer, size_t threadCountPerChannel, Window& window, const std::vector<EqualizerInfo>& infos);

		/**
		 * multithreaded equalizer.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 * @param infos contains the equalizer characteristics.
		 */
		static void EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, Window& window, const std::vector<EqualizerInfo>& infos);

		/**
		 * multithreaded equalizer.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 * @param infos contains the equalizer characteristics.
		 */
		static void EqualizerMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, size_t threadCountPerChannel, Window& window, const std::vector<EqualizerInfo>& infos);

		/**
		 * changes the playback speed of the audio data without changing the pitch.
		 * 
		 * @param speed playback speed multiplier.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void ChangeSpeed(AudioBuffer& buffer, double speed, Window& window);

		/**
		 * changes the playback speed of the audio data without changing the pitch.
		 * 
		 * @param hopSize hop size.
		 * @param windowSize window size.
		 * @param speed playback speed multiplier.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void ChangeSpeed(AudioBuffer& buffer, size_t hopSize, size_t windowSize, double speed, Window& window);

		/**
		 * changes the pitch of the audio data without changing the playback speed.
		 * 
		 * @param pitchChange_semitone change in pitch in terms of semitones.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void PitchShift(AudioBuffer& buffer, double pitchChange_semitone, Window& window);

		/**
		 * changes the pitch of the audio data without changing the playback speed.
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param pitchChange_semitone change in pitch in terms of semitones.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void PitchShift(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, Window& window);

		/**
		 * [multithreaded] changes the pitch of the audio data without changing the playback speed.
		 * 
		 * @param pitchChange_semitone change in pitch in terms of semitones.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void PitchShiftMT(AudioBuffer& buffer, double pitchChange_semitone, Window& window);

		/**
		 * [multithreaded] changes the pitch of the audio data without changing the playback speed.
		 * 
		 * @param pitchChange_semitone change in pitch in terms of semitones.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void PitchShiftMT(AudioBuffer& buffer, double pitchChange_semitone, size_t threadCountPerChannel, Window& window);

		/**
		 * [multithreaded] changes the pitch of the audio data without changing the playback speed.
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param pitchChange_semitone change in pitch in terms of semitones.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, Window& window);

		/**
		 * [multithreaded] changes the pitch of the audio data without changing the playback speed.
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param pitchChange_semitone change in pitch in terms of semitones.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void PitchShiftMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double pitchChange_semitone, size_t threadCountPerChannel, Window& window);
#pragma endregion
#pragma region Filters
	public:
		/**
		 * removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void LowPassFilter(AudioBuffer& buffer, double cutoffFreq, Window& window);

		/**
		 * removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void LowPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void LowPassFilterMT(AudioBuffer& buffer, double cutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void LowPassFilterMT(AudioBuffer& buffer, double cutoffFreq, size_t threadCountPerChannel, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void LowPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, size_t threadCountPerChannel, Window& window);

		/**
		 * removes the frequencies that are lower than the <b>cutoffFreq</b>.
		 * 
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void HighPassFilter(AudioBuffer& buffer, double cutoffFreq, Window& window);

		/**
		 * removes the frequencies that are lower than the <b>cutoffFreq</b>.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void HighPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void HighPassFilterMT(AudioBuffer& buffer, double cutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void HighPassFilterMT(AudioBuffer& buffer, double cutoffFreq, size_t threadCountPerChannel, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are higher than the <b>cutoffFreq</b>.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param cutoffFreq the cutoff frequency in Hz.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, size_t threadCountPerChannel, Window& window);

		/**
		 * removes the frequencies that are not in the provided frequency band.
		 * 
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandPassFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window);

		/**
		 * removes the frequencies that are not in the provided frequency band.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandPassFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are not in the provided frequency band.
		 * 
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandPassFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are not in the provided frequency band.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are not in the provided frequency band.
		 * 
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandPassFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are not in the provided frequency band.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window);
		
		/**
		 * removes the frequencies that are in the provided frequency band.
		 * 
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandCutFilter(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window);

		/**
		 * removes the frequencies that are in the provided frequency band.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandCutFilter(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are in the provided frequency band.
		 * 
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandCutFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are in the provided frequency band.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are in the provided frequency band.
		 * 
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandCutFilterMT(AudioBuffer& buffer, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window);

		/**
		 * [multithreaded] removes the frequencies that are in the provided frequency band.
		 * 
		 * @param hopSize hop size.
		 * @param fftSize length of the FFT in samples.
		 * @param lowCutoffFreq start frequency of the passband.
		 * @param highCutoffFreq stop frequency of the passband.
		 * @param threadCountPerChannel number of threads will be used for each channel.
		 * @param window \link HephAudio::Window Window \endlink that will be applied to the audio signal.
		 */
		static void BandCutFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double lowCutoffFreq, double highCutoffFreq, size_t threadCountPerChannel, Window& window);
#pragma endregion
	};
}