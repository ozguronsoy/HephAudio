#pragma once
#include "HephAudioShared.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief container for equalizer effect parameters.
	 * 
	 */
	struct HEPH_API EqualizerInfo
	{
		/**
		 * first frequency in Hz.
		 * 
		 */
		double f1;

		/**
		 * second frequency in Hz.
		 * 
		 */
		double f2;

		/**
		 * multiplication factor of the frequency components between \a f1 and \a f2.
		 * 
		 */
		double amplitude;
		
		/** @copydoc default_constructor */
		EqualizerInfo();

		/**
		 * @copydoc constructor
		 * 
		 * @param f1 @copydetails f1
		 * @param f2 @copydetails f2
		 * @param amplitude @copydetails amplitude
		 */
		EqualizerInfo(double f1, double f2, double amplitude);
	};
}