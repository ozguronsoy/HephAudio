#pragma once
#include "framework.h"

namespace HephAudio
{
	namespace Structs
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
			void SetThreshold(double positive, double negative);
			void GetThreshold(double& positive, double& negative) const;
			double Distort(double sample) const;
		};
	}
}