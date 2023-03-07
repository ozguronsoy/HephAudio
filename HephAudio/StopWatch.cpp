#include "StopWatch.h"

namespace HephAudio
{
	std::map<std::thread::id, std::chrono::steady_clock::time_point> StopWatch::timePoints = {};
	void StopWatch::Start()
	{
		std::thread::id currentThreadId = std::this_thread::get_id();
		if (timePoints.find(currentThreadId) == timePoints.end())
		{
			timePoints[currentThreadId] = std::chrono::high_resolution_clock::now();
		}
	}
	void StopWatch::Reset()
	{
		timePoints[std::this_thread::get_id()] = std::chrono::high_resolution_clock::now();
	}
	hephaudio_float StopWatch::DeltaTime()
	{
		return (std::chrono::high_resolution_clock::now() - timePoints[std::this_thread::get_id()]).count() * 1e-9;
	}
	hephaudio_float StopWatch::DeltaTime(hephaudio_float prefix)
	{
		return (std::chrono::high_resolution_clock::now() - timePoints[std::this_thread::get_id()]).count() * 1e-9 / prefix;
	}
	void StopWatch::Stop()
	{
		std::thread::id currentThreadId = std::this_thread::get_id();
		if (timePoints.find(currentThreadId) != timePoints.end())
		{
			timePoints.erase(currentThreadId);
		}
	}
}