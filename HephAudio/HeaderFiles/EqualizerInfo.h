#pragma once
#include "HephAudioShared.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief container for equalizer effect parameters.
	 * 
	 */
	struct EqualizerInfo
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
		
		EqualizerInfo();
		EqualizerInfo(double f1, double f2, double amplitude);
	};
}