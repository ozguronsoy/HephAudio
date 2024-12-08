#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief applies soft-clipping distortion via arctan function.
	 * 
	 */
	class HEPH_API ArctanDistortion : public AudioEffect
	{
	protected:
		/**
		 * controls the amount of distortion.
		 * Must be non-negative.
		 * 
		 */
		double factor;

	public:
		/** @copydoc default_constructor */
		ArctanDistortion();

		/**
		 * @copydoc constructor
		 *
		 * @param factor @copydetails factor
		 *
		 */
		explicit ArctanDistortion(double factor);

		/** @copydoc destructor */
		virtual ~ArctanDistortion() = default;

		virtual std::string Name() const override;

		/**
		 * gets the factor.
		 * 
		 */
		virtual double GetFactor() const;

		/**
		 * sets the facotr.
		 * 
		 * @param factor @copydetails factor 
		 */
		virtual void SetFactor(double factor);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}