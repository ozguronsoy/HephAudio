#include "EqualizerInfo.h"

namespace HephAudio
{
	EqualizerInfo::EqualizerInfo() : EqualizerInfo(0.0, 0.0, [](hephaudio_float frequency) -> hephaudio_float { return 0.0; }) {}
	EqualizerInfo::EqualizerInfo(hephaudio_float f1, hephaudio_float f2, hephaudio_float(*amplitudeFunction)(hephaudio_float binFrequency))
		: f1(f1), f2(f2), amplitudeFunction(amplitudeFunction) { }
}