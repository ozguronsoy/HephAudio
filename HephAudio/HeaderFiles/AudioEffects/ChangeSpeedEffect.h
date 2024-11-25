#pragma once
#include "HephAudioShared.h"
#include "OlaEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief changes the playback speed of the audio data without changing the pitch.
	 * 
	 */
	class ChangeSpeedEffect : public OlaEffect
	{
	protected:
		/**
		 * playback speed factor.
		 * 
		 */
		double speed;

	public:
		/** @copydoc default_constructor */
		ChangeSpeedEffect();

		/**
		 * @copydoc constructor
		 * 
		 * @param speed @copydetails speed
		 * 
		 */
		explicit ChangeSpeedEffect(double speed);

		/**
		 * @copydoc constructor
		 *
		 * @param speed @copydetails speed
		 * @param hopSize @copydetails hopSize
		 *
		 */
		ChangeSpeedEffect(double speed, size_t hopSize);

		/**
		 * @copydoc constructor
		 *
		 * @param speed @copydetails speed
		 * @param hopSize @copydetails hopSize
		 * @param wnd @copydetails wnd
		 *
		 */
		ChangeSpeedEffect(double speed, size_t hopSize, const Window& wnd);

		/** @copydoc destructor */
		virtual ~ChangeSpeedEffect() = default;

		virtual std::string Name() const override;
		virtual bool HasRTSupport() const override;
		virtual size_t CalculateRequiredFrameCount(size_t outputFrameCount, const AudioFormatInfo& formatInfo) const override;
		virtual size_t CalculateOutputFrameCount(size_t inputFrameCount, const AudioFormatInfo& formatInfo) const override;

		/**
		 * gets the speed factor.
		 * 
		 */
		virtual double GetSpeed() const;

		/**
		 * sets the speed factor.
		 * 
		 * @param speed @copydetails speed
		 *
		 */
		virtual void SetSpeed(double speed);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}