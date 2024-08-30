#pragma once
#include "HephCommonShared.h"
#include <chrono>

#define HEPH_SW_NANO	(1e-9)
#define HEPH_SW_MICRO	(1e-6)
#define HEPH_SW_MILLI	(1e-3)

#define HEPH_SW_RESET	HephCommon::Stopwatch::Instance.Reset()
#define HEPH_SW_DT		HephCommon::Stopwatch::Instance.DeltaTime()
#define HEPH_SW_DT_S	HephCommon::Stopwatch::Instance.DeltaTime()
#define HEPH_SW_DT_MS	HephCommon::Stopwatch::Instance.DeltaTime(HEPH_SW_MILLI)
#define HEPH_SW_DT_US	HephCommon::Stopwatch::Instance.DeltaTime(HEPH_SW_MICRO)
#define HEPH_SW_DT_NS	HephCommon::Stopwatch::Instance.DeltaTime(HEPH_SW_NANO)

namespace HephCommon
{
	class Stopwatch final
	{
	private:
		std::chrono::steady_clock::time_point tp;
	public:
		static thread_local Stopwatch Instance;
	public:
		Stopwatch();
		void Reset();
		double DeltaTime() const;
		double DeltaTime(double prefix) const;
	};
}