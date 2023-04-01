#pragma once
#include <chrono>
#include <map>
#include <thread>

namespace HephCommon
{
	class StopWatch final
	{
	public:
		static constexpr double nano = 1e-9;
		static constexpr double micro = 1e-6;
		static constexpr double milli = 1e-3;
	private:
		static std::map<std::thread::id, std::chrono::steady_clock::time_point> timePoints;
	public:
		StopWatch() = delete;
		StopWatch(const StopWatch&) = delete;
		StopWatch& operator=(const StopWatch&) = delete;
		static void Start();
		static void Reset();
		static double DeltaTime();
		static double DeltaTime(double prefix);
		static void Stop();
	};
}