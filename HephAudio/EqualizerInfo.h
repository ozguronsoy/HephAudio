#pragma once
#include "framework.h"

namespace HephAudio
{
	struct EqualizerInfo
	{
		/// <summary>
		/// The first frequency in Hz.
		/// </summary>
		hephaudio_float f1;
		/// <summary>
		/// The second frequency in Hz.
		/// </summary>
		hephaudio_float f2;
		/// <summary>
		/// The function that will be called at each frequency bin between f1 and f2.
		/// </summary>
		hephaudio_float (*amplitudeFunction)(hephaudio_float binFrequency);
		EqualizerInfo();
		EqualizerInfo(hephaudio_float f1, hephaudio_float f2, hephaudio_float (*amplitudeFunction)(hephaudio_float binFrequency));
	};
}