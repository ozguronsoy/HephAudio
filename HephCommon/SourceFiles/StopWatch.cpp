#include "StopWatch.h"

namespace HephCommon
{
	thread_local StopWatch StopWatch::Instance = StopWatch();

	StopWatch::StopWatch() : tp(std::chrono::steady_clock::now()) {}
	
	void StopWatch::Reset()
	{
		this->tp = std::chrono::steady_clock::now();
	}
	
	double StopWatch::DeltaTime() const
	{
		return (std::chrono::steady_clock::now() - this->tp).count() * 1e-9;
	}
	
	double StopWatch::DeltaTime(double prefix) const
	{
		return (std::chrono::steady_clock::now() - this->tp).count() * 1e-9 / prefix;
	}
}