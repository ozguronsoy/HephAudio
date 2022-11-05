#pragma once
namespace HephAudio
{
	struct EqualizerInfo
	{
		double f1;
		double f2;
		double (*volumeFunction)(double frequency);
		EqualizerInfo();
		EqualizerInfo(double f1, double f2, double (*volumeFunction)(double frequency));
	};
}