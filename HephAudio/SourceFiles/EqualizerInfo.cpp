#include "EqualizerInfo.h"

namespace HephAudio
{
	EqualizerInfo::EqualizerInfo() : EqualizerInfo(0.0, 0.0, 0.0) {}
	EqualizerInfo::EqualizerInfo(heph_float f1, heph_float f2, heph_float amplitude) : f1(f1), f2(f2), amplitude(amplitude) { }
}