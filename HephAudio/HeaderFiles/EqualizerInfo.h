#pragma once
#include "HephAudioShared.h"

namespace HephAudio
{
	struct EqualizerInfo
	{
		double f1;
		double f2;
		double amplitude;
		EqualizerInfo();
		EqualizerInfo(double f1, double f2, double amplitude);
	};
}