#pragma once
#include "HephAudioFramework.h"

namespace HephAudio
{
	/// <summary>
	/// Holds the necessary information for an equalizer effect to be applied.
	/// </summary>
	struct EqualizerInfo
	{
		/// <summary>
		/// The first frequency in Hz.
		/// </summary>
		heph_float f1;
		/// <summary>
		/// The second frequency in Hz.
		/// </summary>
		heph_float f2;
		/// <summary>
		/// The function that will be called at each frequency bin between f1 and f2.
		/// </summary>
		heph_float (*amplitudeFunction)(heph_float binFrequency);
		/// <summary>
		/// Creates and initializes an EqualizerInfo instance with the default values.
		/// </summary>
		EqualizerInfo();
		/// <summary>
		/// Creates and initializes an EqualizerInfo instance with the provided values.
		/// </summary>
		/// <param name="f1">The first frequency in Hz.</param>
		/// <param name="f2">The second frequency in Hz.</param>
		/// <param name="amplitudeFunction">The function that will be called at each frequency bin between f1 and f2.</param>
		EqualizerInfo(heph_float f1, heph_float f2, heph_float (*amplitudeFunction)(heph_float binFrequency));
	};
}