#pragma once
#include "HephAudioShared.h"
#include "ModulationEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief delays the audio data and mixes the result with the input signal. 
	 * The amount of delay applied changes periodically.
	 */
	class HEPH_API Flanger : public ModulationEffect
	{
	public:
		using ModulationEffect::Process;

	protected:
		/**
		 * constant delay in milliseconds.
		 */
		double constantDelay;
		
		/**
		 * maximum value of the variable delay in milliseconds. 
		 */
		double variableDelay;

		/**
		 * past samples required for real-time processing.
		 */
		AudioBuffer pastSamples;

	public:
		/** @copydoc default_constructor */
		Flanger();

		/**
		 * @copydoc ModulationEffect(double, const Oscillator&)
		 *
		 * @param constantDelay @copydetails constantDelay
		 * @param variableDelay @copydetails variableDelay
		 * 
		 */
		Flanger(double depth, double constantDelay, double variableDelay, const Oscillator& lfo);

		/** @copydoc destructor */
		virtual ~Flanger() = default;

		virtual std::string Name() const override;
		virtual void ResetInternalState() override;
		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

		/**
		 * gets the constant delay in milliseconds.
		 *
		 */
		virtual double GetConstantDelay() const;

		/**
		 * sets the constant delay in milliseconds.
		 * 
		 * @param constantDelay @copydetails constantDelay
		 */
		virtual void SetConstantDelay(double constantDelay);
		
		/**
		 * gets the variable delay in milliseconds.
		 * 
		 */
		virtual double GetVariableDelay() const;

		/**
		 * sets the variable delay in milliseconds.
		 *
		 * @param variableDelay @copydetails variableDelay
		 */
		virtual void SetVariableDelay(double variableDelay);

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