#pragma once
#include "HephAudioShared.h"
#include "Flanger.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief delays the audio data and changes its pitch periodically. 
	 * Then mixes the result with the input signal.
	 * 
	 * @note this is a non-causal effect.
	 */
	class Chorus : public Flanger
	{
	protected:
		/**
		 * maximum pitch change in terms of semitones.
		 */
		double extent;

	public:
		/** @copydoc default_constructor */
		Chorus();

		/**
		 * @copydoc Flanger(double, double, double, const Oscillator&)
		 *
		 * @param extent @copydetails extent
		 */
		Chorus(double depth, double constantDelay, double variableDelay, double extent, const Oscillator& lfo);

		/** @copydoc destructor */
		virtual ~Chorus() = default;

		virtual std::string Name() const override;
		virtual size_t CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const override;

		/**
		 * gets the @copydetails extent.
		 *
		 */
		virtual double GetExtent() const;

		/**
		 * sets extent.
		 *
		 * @param extent @copydetails extent
		 *
		 */
		virtual void SetExtent(double extent);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
		virtual size_t CalculatePastSamplesSize(const AudioBuffer& inputBuffer) const override;
	};
}