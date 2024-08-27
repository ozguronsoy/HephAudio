#pragma once
#include "HephCommonShared.h"
#include <chrono>

#define HEPH_SW_NANO (1e-9)
#define HEPH_SW_MICRO (1e-6)
#define HEPH_SW_MILLI (1e-3)

namespace HephCommon
{
	class StopWatch final
	{
	private:
		std::chrono::steady_clock::time_point tp;
	public:
		static thread_local StopWatch Instance;
	public:
		StopWatch();
		void Reset();
		double DeltaTime() const;
		double DeltaTime(double prefix) const;
	};
}