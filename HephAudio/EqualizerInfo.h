#pragma once
#include "framework.h"

namespace HephAudio
{
	struct EqualizerInfo
	{
		hephaudio_float f1;
		hephaudio_float f2;
		hephaudio_float (*amplitudeFunction)(hephaudio_float frequency);
		EqualizerInfo();
		EqualizerInfo(hephaudio_float f1, hephaudio_float f2, hephaudio_float (*amplitudeFunction)(hephaudio_float frequency));
		virtual ~EqualizerInfo() = default;
	};
}