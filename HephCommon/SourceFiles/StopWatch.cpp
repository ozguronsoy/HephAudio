#include "StopWatch.h"

#define TP_ZERO std::chrono::steady_clock::time_point(std::chrono::nanoseconds(0))

namespace HephCommon
{
	std::map<std::thread::id, std::chrono::steady_clock::time_point> StopWatch::timePoints = {};
	StopWatch::StopWatch() : tp(TP_ZERO) {}
	void StopWatch::Start()
	{
		if (this->tp == TP_ZERO)
		{
			this->tp = std::chrono::steady_clock::now();
		}
	}
	void StopWatch::Reset()
	{
		this->tp = std::chrono::steady_clock::now();
	}
	double StopWatch::DeltaTime() const
	{
		if (this->tp != TP_ZERO)
		{
			return (std::chrono::steady_clock::now() - this->tp).count() * 1e-9;
		}
		return -1.0;
	}
	double StopWatch::DeltaTime(double prefix) const
	{
		if (this->tp != TP_ZERO)
		{
			return (std::chrono::steady_clock::now() - this->tp).count() * 1e-9 / prefix;
		}
		return -1.0;
	}
	void StopWatch::Stop()
	{
		tp = TP_ZERO;
	}
	void StopWatch::StaticStart()
	{
		std::thread::id currentThreadId = std::this_thread::get_id();
		if (timePoints.find(currentThreadId) == timePoints.end())
		{
			timePoints[currentThreadId] = std::chrono::steady_clock::now();
		}
	}
	void StopWatch::StaticReset()
	{
		timePoints[std::this_thread::get_id()] = std::chrono::steady_clock::now();
	}
	double StopWatch::StaticDeltaTime()
	{
		return (std::chrono::steady_clock::now() - timePoints[std::this_thread::get_id()]).count() * 1e-9;
	}
	double StopWatch::StaticDeltaTime(double prefix)
	{
		return (std::chrono::steady_clock::now() - timePoints[std::this_thread::get_id()]).count() * 1e-9 / prefix;
	}
	void StopWatch::StaticStop()
	{
		std::thread::id currentThreadId = std::this_thread::get_id();
		if (timePoints.find(currentThreadId) != timePoints.end())
		{
			timePoints.erase(currentThreadId);
		}
	}
}