#include "DistortionInfo.h"

namespace HephAudio
{
	DistortionInfo::DistortionInfo() : DistortionInfo(1.0, -1.0) {}
	DistortionInfo::DistortionInfo(double positive_threshold, double negative_threshold)
	{
		distort = false;
		SetThresholds(positive_threshold, negative_threshold);
	}
	void DistortionInfo::SetThresholds(double positive, double negative)
	{
		if (positive > 1.0) { positive = 1.0; }
		if (positive < 0.0) { positive = 0.0; }
		if (negative > 0.0) { negative = 0.0; }
		if (negative < -1.0) { negative = -1.0; }
		positive_threshold = positive;
		negative_threshold = negative;
	}
	void DistortionInfo::GetThresholds(double& positive, double& negative) const
	{
		positive = positive_threshold;
		negative = negative_threshold;
	}
	double DistortionInfo::Distort(double sample) const
	{
		if (sample > positive_threshold)
		{
			sample = positive_threshold;
		}
		else if (sample < negative_threshold)
		{
			sample = negative_threshold;
		}
		return sample;
	}
}