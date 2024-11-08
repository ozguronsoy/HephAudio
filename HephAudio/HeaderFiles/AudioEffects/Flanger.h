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
	class Flanger final : public ModulationEffect
	{
	public:
		using ModulationEffect::Process;

	private:
		AudioBuffer oldSamples;

	private:
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

		std::string Name() const override;

		void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

		/**
		 * gets the constant delay in milliseconds.
		 *
		 */
		double GetConstantDelay() const;

		/**
		 * sets the constant delay in milliseconds.
		 * 
		 * @param constantDelay @copydetails constantDelay
		 */
		void SetConstantDelay(double constantDelay);
		
		/**
		 * gets the variable delay in milliseconds.
		 * @return 
		 */
		double GetVariableDelay() const;

		/**
		 * sets the variable delay in milliseconds.
		 *
		 * @param variableDelay @copydetails variableDelay
		 */
		void SetVariableDelay(double variableDelay);

	private:
		void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}