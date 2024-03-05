#pragma once
#include "HephAudioShared.h"

namespace HephAudio
{
	struct EqualizerInfo
	{
		heph_float f1;
		heph_float f2;
		heph_float amplitude;
		EqualizerInfo();
		EqualizerInfo(heph_float f1, heph_float f2, heph_float amplitude);
	};
}