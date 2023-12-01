#pragma once
#include "HephAudioFramework.h"

namespace HephAudio
{
	struct EqualizerInfo
	{
		heph_float f1;
		heph_float f2;
		heph_float (*amplitudeFunction)(heph_float binFrequency);
		EqualizerInfo();
		EqualizerInfo(heph_float f1, heph_float f2, heph_float (*amplitudeFunction)(heph_float binFrequency));
	};
}