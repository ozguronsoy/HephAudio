#pragma once
#include "HephCommonFramework.h"
#include <chrono>
#include <map>
#include <thread>

namespace HephCommon
{
	class StopWatch final
	{
	private:
#if defined(__linux__)
		typedef std::chrono::system_clock::time_point TimePoint;
#else
		typedef std::chrono::steady_clock::time_point TimePoint;
#endif
	public:
		static constexpr double nano = 1e-9;
		static constexpr double micro = 1e-6;
		static constexpr double milli = 1e-3;
	private:
		TimePoint tp;
	private:
		static std::map<std::thread::id, TimePoint> timePoints;
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