#pragma once
#include "framework.h"

namespace HephAudio
{
	struct EqualizerInfo
	{
		HEPHAUDIO_DOUBLE f1;
		HEPHAUDIO_DOUBLE f2;
		HEPHAUDIO_DOUBLE (*amplitudeFunction)(HEPHAUDIO_DOUBLE frequency);
		EqualizerInfo();
		EqualizerInfo(HEPHAUDIO_DOUBLE f1, HEPHAUDIO_DOUBLE f2, HEPHAUDIO_DOUBLE (*amplitudeFunction)(HEPHAUDIO_DOUBLE frequency));
		virtual ~EqualizerInfo() = default;
	};
}