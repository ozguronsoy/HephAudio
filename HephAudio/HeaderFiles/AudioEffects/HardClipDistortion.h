#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief applies hard-clipping distortion.
	 * 
	 */
	class HEPH_API HardClipDistortion : public AudioEffect
	{
	protected:
		/**
		 * threshold value.
		 * 
		 */
		heph_audio_sample_t clippingLevel;

	public:
		/** @copydoc default_constructor */
		HardClipDistortion();

		/**
		 * @copydoc constructor
		 *
		 * @param clippingLevel threshold value in decibels.
		 *
		 */
		explicit HardClipDistortion(double clippingLevel);

		/** @copydoc destructor */
		virtual ~HardClipDistortion() = default;

		virtual std::string Name() const override;

		/**
		 * gets the clipping level in decibels.
		 * 
		 */
		virtual double GetClippingLevel() const;

		/**
		 * sets the clipping level.
		 * 
		 * @param clippingLevel threshold value in decibels.
		 */
		virtual void SetClippingLevel(double clippingLevel);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}