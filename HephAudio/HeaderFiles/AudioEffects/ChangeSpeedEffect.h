#pragma once
#include "HephAudioShared.h"
#include "DoubleBufferedAudioEffect.h"
#include "Buffers/DoubleBuffer.h"
#include "Windows/Window.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief changes the playback speed of the audio data without changing the pitch.
	 * 
	 */
	class ChangeSpeedEffect : public DoubleBufferedAudioEffect
	{
	protected:
		/**
		 * playback speed factor.
		 * 
		 */
		double speed;

		/**
		 * number of frames to advance each iteration.
		 * 
		 */
		size_t hopSize;

		/**
		 * window that will be applied to the input.
		 * 
		 */
		Heph::DoubleBuffer wnd;

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

		/**
		 * gets the hop size.
		 * 
		 */
		virtual size_t GetHopSize() const;

		/**
		 * sets the hop size.
		 * 
		 * @param hopSize @copydetails hopSize
		 *
		 */
		virtual void SetHopSize(size_t hopSize);

		/**
		 * gets the size of the window.
		 *
		 */
		virtual size_t GetWindowSize() const;

		/**
		 * sets the window.
		 * 
		 * @param wnd @copydetails wnd.
		 * 
		 */
		virtual void SetWindow(const Window& wnd);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}