#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief applies linear fade-in.
	 * 
	 */
	class HEPH_API LinearFadeIn : public AudioEffect
	{
	public:
		using AudioEffect::Process;

	protected:
		/**
		 * duration of the fade-in in seconds.
		 * 
		 */
		double duration;

		/**
		 * time, in seconds, of the first audio frame the effect will be applied to.
		 * 
		 */
		double startTime;

		/**
		 * for real-time processing.
		 * 
		 */
		size_t currentIndex;

	public:
		/** @copydoc default_constructor */
		LinearFadeIn();

		/**
		 * @copydoc constructor
		 *
		 * @param duration @copydetails duration
		 *
		 */
		explicit LinearFadeIn(double duration);

		/**
		 * @copydoc constructor
		 *
		 * @param duration @copydetails duration
		 * @param startTime @copydetails startTime
		 *
		 */
		LinearFadeIn(double duration, double startTime);

		/** @copydoc destructor */
		virtual ~LinearFadeIn() = default;

		virtual std::string Name() const override;
		virtual void ResetInternalState() override;
		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

		/**
		 * gets the duration in seconds.
		 *
		 */
		virtual double GetDuration() const;

		/**
		 * sets the duration.
		 *
		 * @param duration @copydetails duration
		 */
		virtual void SetDuration(double duration);

		/**
		 * gets the start time in seconds.
		 *
		 */
		virtual double GetStartTime() const;

		/**
		 * sets the start time.
		 *
		 * @param startTime @copydetails startTime
		 */
		virtual void SetStartTime(double startTime);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}