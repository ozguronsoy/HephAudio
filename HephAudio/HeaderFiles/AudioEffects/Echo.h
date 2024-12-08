#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief adds echo to the audio data.
	 *
	 */
	class HEPH_API Echo : public AudioEffect
	{
	public:
		using AudioEffect::Process;

	protected:
		/**
		 * number of times the audio data will be reflected.
		 *
		 */
		size_t reflectionCount;

		/**
		 * elapsed time, in seconds, between the start of each reflection.
		 *
		 */
		double reflectionDelay;

		/**
		 * the factor which echo data will be multiplied by between each reflection.
		 *
		 */
		double decayFactor;

		/**
		 * start time, in seconds, of the audio data that will be used as echo.
		 *
		 */
		double echoStart;

		/**
		 * duration of the audio data, in seconds, that will be used as echo.
		 *
		 */
		double echoDuration;

		/**
		 * past samples required for real-time processing.
		 *
		 */
		AudioBuffer pastSamples;

		/**
		 * for real-time processing.
		 *
		 */
		size_t currentIndex;

	public:
		/** @copydoc default_constructor */
		Echo();

		/**
		 * @copydoc constructor
		 *
		 * @param reflectionCount @copydetails reflectionCount
		 * @param reflectionDelay @copydetails reflectionDelay
		 * @param decayFactor @copydetails decayFactor
		 * @param echoStart @copydetails echoStart
		 * @param echoDuration @copydetails echoDuration
		 *
		 */
		Echo(size_t reflectionCount, double reflectionDelay, double decayFactor, double echoStart, double echoDuration);

		/** @copydoc destructor */
		virtual ~Echo() = default;

		virtual std::string Name() const override;
		virtual void ResetInternalState() override;
		virtual void Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount) override;

		/**
		 * gets the reflection count.
		 *
		 */
		virtual size_t GetReflectionCount() const;

		/**
		 * sets the reflection count.
		 *
		 * @param reflectionCount @copydetails reflectionCount
		 *
		 */
		virtual void SetReflectionCount(size_t reflectionCount);

		/**
		 * gets the reflection delay.
		 *
		 */
		virtual double GetReflectionDelay() const;

		/**
		 * sets the reflection delay.
		 *
		 * @param reflectionDelay @copydetails reflectionDelay
		 *
		 */
		virtual void SetReflectionDelay(double reflectionDelay);

		/**
		 * gets the decay factor.
		 *
		 */
		virtual double GetDecayFactor() const;

		/**
		 * sets the decay factor.
		 *
		 * @param decayFactor @copydetails decayFactor
		 *
		 */
		virtual void SetDecayFactor(double decayFactor);

		/**
		 * gets the echo start.
		 *
		 */
		virtual double GetEchoStart() const;

		/**
		 * sets the echo start.
		 *
		 * @param echoStart @copydetails echoStart
		 *
		 */
		virtual void SetEchoStart(double echoStart);

		/**
		 * gets the echo duration.
		 *
		 */
		virtual double GetEchoDuration() const;

		/**
		 * sets the echo duration.
		 *
		 * @param echoDuration @copydetails echoDuration
		 *
		 */
		virtual void SetEchoDuration(double echoDuration);

	protected:
		void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}