#include "EqualizerInfo.h"

namespace HephAudio
{
	EqualizerInfo::EqualizerInfo() : EqualizerInfo(0.0, 0.0, 0.0) {}
	EqualizerInfo::EqualizerInfo(double f1, double f2, double amplitude) : f1(f1), f2(f2), amplitude(amplitude) { }
}