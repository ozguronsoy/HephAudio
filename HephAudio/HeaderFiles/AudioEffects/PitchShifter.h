#pragma once
#include "HephAudioShared.h"
#include "OlaEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief changes the pitch without changing the playback speed.
	 * 
	 */
	class HEPH_API PitchShifter : public OlaEffect
	{
	protected:
		/**
		 * frequency multiplier.
		 * 
		 */
		double pitchFactor;

	public:
		/** @copydoc default_constructor */
		PitchShifter();

		/** @copydoc FrequencyDomainEffect(size_t, const Window&) */
		PitchShifter(size_t hopSize, const Window& wnd);

		/**
		 * @copydoc constructor
		 * 
		 * @param toneChange in semitones. Positive value raises the pitch, negative value lowers it.
		 * @param hopSize @copydetalis hopSize
		 * @param wnd @copydetalis wnd
		 * 
		 */
		PitchShifter(double toneChange, size_t hopSize, const Window& wnd);

		/** @copydoc destructor */
		virtual ~PitchShifter() = default;

		virtual std::string Name() const override;

		/**
		 * gets the tone change in semitones.
		 * 
		 */
		virtual double GetToneChange() const;

		/**
		 * sets the tone change in semitones.
		 *
		 * @param toneChange in semitones. Positive value raises the pitch, negative value lowers it.
		 *
		 */
		virtual void SetToneChange(double toneChange);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}