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
	class Flanger : public ModulationEffect
	{
	public:
		using ModulationEffect::Process;

	protected:
		/**
		 * past samples required for real-time processing.
		 */
		AudioBuffer pastSamples;

		/**
		 * constant delay in milliseconds.
		 */
		double constantDelay;
		
		/**
		 * maximum value of the variable delay in milliseconds. 
		 */
		double variableDelay;

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
	};
}