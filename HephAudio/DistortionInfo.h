#pragma once
#include "framework.h"

namespace HephAudio
{
	struct DistortionInfo
	{
	private:
		double negative_threshold;
		double positive_threshold;
	public:
		bool distort;
	public:
		DistortionInfo();
		DistortionInfo(double positive_threshold, double negative_threshold);
		virtual ~DistortionInfo() = default;
		void SetThresholds(double positive, double negative);
		void GetThresholds(double& positive, double& negative) const;
		double Distort(double sample) const;
	};
}