#include "EqualizerInfo.h"

namespace HephAudio
{
	EqualizerInfo::EqualizerInfo() : EqualizerInfo(0.0, 0.0, [](HEPHAUDIO_DOUBLE frequency) -> HEPHAUDIO_DOUBLE { return 1.0; }) {}
	EqualizerInfo::EqualizerInfo(HEPHAUDIO_DOUBLE f1, HEPHAUDIO_DOUBLE f2, HEPHAUDIO_DOUBLE (*amplitudeFunction)(HEPHAUDIO_DOUBLE frequency))
	{
		this->f1 = f1;
		this->f2 = f2;
		this->amplitudeFunction = amplitudeFunction;
	}
}