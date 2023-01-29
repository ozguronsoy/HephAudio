#include "StopWatch.h"

namespace HephAudio
{
	std::map<std::thread::id, std::chrono::steady_clock::time_point> StopWatch::timePoints = {};
	void StopWatch::Start()
	{
		std::thread::id currentThreadId = std::this_thread::get_id();
		if (!timePoints.contains(currentThreadId))
		{
			timePoints[currentThreadId] = clock.now();
		}
	}
	void StopWatch::Reset()
	{
		timePoints[std::this_thread::get_id()] = clock.now();
	}
	double StopWatch::DeltaTime()
	{
		return (clock.now() - timePoints[std::this_thread::get_id()]).count() * 1e-9;
	}
	double StopWatch::DeltaTime(double prefix)
	{
		return (clock.now() - timePoints[std::this_thread::get_id()]).count() * 1e-9 / prefix;
	}
	void StopWatch::Stop()
	{
		std::thread::id currentThreadId = std::this_thread::get_id();
		if (timePoints.contains(currentThreadId))
		{
			timePoints.erase(currentThreadId);
		}
	}
}