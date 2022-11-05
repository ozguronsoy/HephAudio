#include "EqualizerInfo.h"
namespace HephAudio
{
	EqualizerInfo::EqualizerInfo() : EqualizerInfo(0.0, 0.0, [](double frequency) { return 1.0; }) {}
	EqualizerInfo::EqualizerInfo(double f1, double f2, double (*volumeFunction)(double frequency))
	{
		this->f1 = f1;
		this->f2 = f2;
		this->volumeFunction = volumeFunction;
	}
}