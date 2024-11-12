#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief applies linear fade-out.
	 *
	 */
	class LinearFadeOut : public AudioEffect
	{
	public:
		using AudioEffect::Process;

	protected:
		/**
		 * duration of the fade-out in seconds.
		 *
		 */
		double duration;

		/**
		 * index of the first audio frame the effect will be applied to.
		 *
		 */
		size_t startIndex;

		/**
		 * for real-time processing.
		 *
		 */
		size_t currentIndex;

	public:
		/** @copydoc default_constructor */
		LinearFadeOut();

		/**
		 * @copydoc constructor
		 *
		 * @param duration @copydetails duration
		 *
		 */
		explicit LinearFadeOut(double duration);

		/**
		 * @copydoc constructor
		 *
		 * @param duration @copydetails duration
		 * @param startIndex @copydetails startIndex
		 *
		 */
		LinearFadeOut(double duration, size_t startIndex);

		/** @copydoc destructor */
		virtual ~LinearFadeOut() = default;

		virtual std::string Name() const override;
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
		 * gets the start index.
		 *
		 */
		virtual size_t GetStartIndex() const;

		/**
		 * sets the start index.
		 *
		 * @param startIndex @copydetails startIndex
		 */
		virtual void SetStartIndex(size_t startIndex);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}