#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"
#include <mutex>
#include <condition_variable>

/** @file */

namespace HephAudio
{
	/**
	 * @brief normalizes the audio data at the provided RMS.
	 */
	class RmsNormalizer : public AudioEffect
	{
	protected:
		/**
		 * desired RMS value for the output signal.
		 *
		 */
		double targetRms;

		/**
		 * RMS value of the current input signal.
		 *
		 */
		double currentRms;

		/**
		 * last gain for smooth transition.
		 *
		 */
		double lastGain;

		/**
		 * factor for smoothing the gain transition.
		 * In range of [0, 1) where 0 means no smoothing is applied.
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
		RmsNormalizer();

		/**
		 * @copydoc constructor
		 *
		 * @param targetRms @copydetails targetRms
		 *
		 */
		explicit RmsNormalizer(heph_audio_sample_t targetRms);

		/**
		 * @copydoc constructor
		 *
		 * @param peakAmplitude @copydetails peakAmplitude
		 * @param smoothingFactor @copydetails smoothingFactor
		 *
		 */
		RmsNormalizer(heph_audio_sample_t peakAmplitude, double smoothingFactor);

		/** @copydoc destructor */
		virtual ~RmsNormalizer() = default;

		virtual std::string Name() const override;

		/**
		 * gets the target RMS.
		 *
		 */
		virtual double GetTargetRms() const;

		/**
		 * sets the target RMS.
		 *
		 * @param targetRms @copydetails targetRms
		 *
		 */
		virtual void SetTargetRms(double targetRms);

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