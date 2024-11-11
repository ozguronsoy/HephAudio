#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief applies cubic distortion.
	 * 
	 */
	class CubicDistortion : public AudioEffect
	{
	protected:
		/**
		 * controls the amount of distortion.
		 * In the range of [0, 10].
		 *
		 */
		double factor;

	public:
		/** @copydoc default_constructor */
		CubicDistortion();

		/**
		 * @copydoc constructor
		 *
		 * @param factor @copydetails factor
		 *
		 */
		explicit CubicDistortion(double factor);

		/** @copydoc destructor */
		virtual ~CubicDistortion() = default;

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