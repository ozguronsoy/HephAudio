#pragma once
#include "framework.h"
#include <chrono>
#include <map>
#include <thread>

namespace HephAudio
{
	class StopWatch final
	{
	public:
		static constexpr HEPHAUDIO_DOUBLE nano = 1e-9;
		static constexpr HEPHAUDIO_DOUBLE micro = 1e-6;
		static constexpr HEPHAUDIO_DOUBLE milli = 1e-3;
	private:
		static std::map<std::thread::id, std::chrono::steady_clock::time_point> timePoints;
	public:
		StopWatch() = delete;
		StopWatch(const StopWatch&) = delete;
		StopWatch& operator=(const StopWatch&) = delete;
		static void Start();
		static void Reset();
		static HEPHAUDIO_DOUBLE DeltaTime();
		static HEPHAUDIO_DOUBLE DeltaTime(HEPHAUDIO_DOUBLE prefix);
		static void Stop();
	};
}