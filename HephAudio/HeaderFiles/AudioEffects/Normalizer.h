#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"
#include <mutex>
#include <condition_variable>

/** @file */

namespace HephAudio
{
	/**
	 * @brief normalizes the audio data at the provided amplitude.
	 */
	class Normalizer : public AudioEffect
	{
	protected:
		/**
		 * maximum amplitude of the output signal.
		 * 
		 */
		heph_audio_sample_t peakAmplitude;

		/**
		 * the maximum sample the input buffer contains.
		 * 
		 */
		heph_audio_sample_t globalMaxSample;

		/**
		 * last gain for smooth transition.
		 * 
		 */
		double lastGain;

		/**
		 * factor for smoothing the gain transition.
		 * In the range of [0, 1) where 0 means no smoothing is applied.
		 * 
		 */
		double smoothingFactor;

		/**
		 * for editing the globalMaxSample.
		 * 
		 */
		std::mutex mutex;

		/**
		 * for waiting all threads to finish updating the globalMaxSample.
		 * 
		 */
		std::condition_variable cv;

		/**
		 * number of threads completed updating the globalMaxSample.
		 * 
		 */
		size_t threadsCompleted;

	public:
		/** @copydoc default_constructor */
		Normalizer();

		/** 
		 * @copydoc constructor
		 * 
		 * @param peakAmplitude @copydetails peakAmplitude
		 * 
		 */
		explicit Normalizer(heph_audio_sample_t peakAmplitude);

		/**
		 * @copydoc constructor
		 * 
		 * @param peakAmplitude @copydetails peakAmplitude
		 * @param smoothingFactor @copydetails smoothingFactor
		 * 
		 */
		Normalizer(heph_audio_sample_t peakAmplitude, double smoothingFactor);

		/** @copydoc destructor */
		virtual ~Normalizer() = default;

		virtual std::string Name() const override;

		/**
		 * gets the peak amplitude
		 * 
		 */
		virtual heph_audio_sample_t GetPeakAmplitude() const;
		
		/**
		 * sets the peak amplitude.
		 * 
		 * @param peakAmplitude @copydetails peakAmplitude
		 * 
		 */
		virtual void SetPeakAmplitude(heph_audio_sample_t peakAmplitude);

		/**
		 * gets the smoothing factor.
		 * 
		 */
		virtual double GetSmoothingFactor() const;

		/**
		 * sets the smoothing factor.
		 * 
		 * @param smoothingFactor @copydetails smoothingFactor
		 * 
		 */
		virtual void SetSmoothingFactor(double smoothingFactor);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}