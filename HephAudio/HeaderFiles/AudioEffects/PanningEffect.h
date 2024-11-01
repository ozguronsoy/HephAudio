#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for panning effects.
	 * 
	 */
	class PanningEffect : public AudioEffect
	{
	public:
		using AudioEffect::Process;

	protected:
		/**
		 * multiplication factor for the left channel.
		 * 
		 */
		double leftVolume;

		/**
		 * multiplication factor for the right channel.
		 *
		 */
		double rightVolume;

	protected:
		/** @copydoc default_constructor */
		PanningEffect();

	public:
		/** @copydoc destructor */
		virtual ~PanningEffect() = default;

		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

		/**
		 * gets the panning factor.
		 *
		 */
		virtual double GetFactor() const = 0;

		/**
		 * sets the panning factor.
		 *
		 * @param factor determines how the channels' volume will be changed.
		 * Ranges between -1 and 1.<br>
		 * -1 means only the left channel will be audible and 1 means only the right channel will be audible.
		 *
		 */
		virtual void SetFactor(double factor) = 0;

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t endIndex) override;
	};
}