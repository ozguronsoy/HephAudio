#pragma once
#include "HephAudioShared.h"
#include "PanningEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief linearly increases the volume of one channel while decreasing the other one (stereo only).
	 */
	class HEPH_API LinearPanning : public PanningEffect
	{
	public:
		/** @copydoc default_constructor */
		LinearPanning();

		/**
		 * @copydoc constructor
		 * @param factor determines how the channels' volume will be changed.
		 * Ranges between -1 and 1.<br>
		 * -1 means only the left channel will be audible and 1 means only the right channel will be audible.
		 *
		 */
		explicit LinearPanning(double factor);

		/** @copydoc destructor */
		virtual ~LinearPanning() = default;

		virtual std::string Name() const override;
		virtual double GetFactor() const override;
		virtual void SetFactor(double factor) override;
	};
}