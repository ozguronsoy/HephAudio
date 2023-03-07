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
		static constexpr hephaudio_float nano = 1e-9;
		static constexpr hephaudio_float micro = 1e-6;
		static constexpr hephaudio_float milli = 1e-3;
	private:
		static std::map<std::thread::id, std::chrono::steady_clock::time_point> timePoints;
	public:
		StopWatch() = delete;
		StopWatch(const StopWatch&) = delete;
		StopWatch& operator=(const StopWatch&) = delete;
		static void Start();
		static void Reset();
		static hephaudio_float DeltaTime();
		static hephaudio_float DeltaTime(hephaudio_float prefix);
		static void Stop();
	};
}