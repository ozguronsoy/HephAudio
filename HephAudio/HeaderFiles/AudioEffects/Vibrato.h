#pragma once
#include "HephAudioShared.h"
#include "ModulationEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief changes the pitch of the audio data periodically and mixes the result with the input signal.
	 * 
	 * @note this is a non-causal effect.
	 */
	class HEPH_API Vibrato : public ModulationEffect
	{
	public:
		using ModulationEffect::Process;

	protected:
		/**
		 * maximum pitch change in terms of semitones.
		 */
		double extent;

		/**
		 * past samples required for real-time processing.
		 */
		AudioBuffer pastSamples;

	public:
		/** @copydoc default_constructor */
		Vibrato();

		/**
		 * @copydoc ModulationEffect(double, const Oscillator&) 
		 * 
		 * @param extent @copydetails extent
		 */
		Vibrato(double depth, double extent, const Oscillator& lfo);

		/** @copydoc destructor */
		virtual ~Vibrato() = default;

		virtual std::string Name() const override;
		virtual size_t CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const override;
		virtual void ResetInternalState() override;
		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

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

		/**
		 * calculates the number of past samples required.
		 *
		 * @param inputBuffer contains the dry data.
		 * 
		 */
		virtual size_t CalculatePastSamplesSize(const AudioBuffer& inputBuffer) const;
	};
}