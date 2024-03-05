#pragma once
#include "HephCommonShared.h"
#include <chrono>
#include <map>
#include <thread>

#define HEPH_SW_NANO 1e-9
#define HEPH_SW_MICRO 1e-6
#define HEPH_SW_MILLI 1e-3

namespace HephCommon
{
	class StopWatch final
	{
	private:
		std::chrono::steady_clock::time_point tp;
	private:
		static std::map<std::thread::id, std::chrono::steady_clock::time_point> timePoints;
	public:
		StopWatch();
		void Start();
		void Reset();
		double DeltaTime() const;
		double DeltaTime(double prefix) const;
		void Stop();
		static void StaticStart();
		static void StaticReset();
		static double StaticDeltaTime();
		static double StaticDeltaTime(double prefix);
		static void StaticStop();
	};
}