#pragma once
#include "HephAudioShared.h"
#include "PanningEffect.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief linearly increases the volume of one channel while decreasing the other one (stereo only).
	 */
	class LinearPanningEffect final : public PanningEffect
	{
	public:
		/** @copydoc default_constructor */
		LinearPanningEffect();

		/**
		 * @copydoc constructor
		 * @param factor determines how the channels' volume will be changed.
		 * Ranges between -1 and 1.<br>
		 * -1 means only the left channel will be audible and 1 means only the right channel will be audible.
		 *
		 */
		explicit LinearPanningEffect(double factor);

		std::string Name() const override;
		double GetFactor() const override;
		void SetFactor(double factor) override;
	};
}