#pragma once
#include "HephAudioShared.h"
#include "AudioEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief applies overdrive distortion.
	 *
	 */
	class HEPH_API Overdrive : public AudioEffect
	{
	protected:
		/**
		 * controls the amount of distortion.
		 * In the range of [0, 100]
		 * 
		 */
		double drive;

	public:
		/** @copydoc default_constructor */
		Overdrive();

		/**
		 * @copydoc constructor
		 *
		 * @param drive @copydetails drive
		 *
		 */
		explicit Overdrive(double drive);

		/** @copydoc destructor */
		virtual ~Overdrive() = default;

		virtual std::string Name() const override;

		/**
		 * gets the drive.
		 *
		 */
		virtual double GetDrive() const;

		/**
		 * sets the drive.
		 *
		 * @param drive @copydetails drive
		 */
		virtual void SetDrive(double drive);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
	};
}