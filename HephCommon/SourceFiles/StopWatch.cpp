#include "Stopwatch.h"

namespace HephCommon
{
	thread_local Stopwatch Stopwatch::Instance = Stopwatch();

	Stopwatch::Stopwatch() : tp(std::chrono::steady_clock::now()) {}
	
	void Stopwatch::Reset()
	{
		this->tp = std::chrono::steady_clock::now();
	}
	
	double Stopwatch::DeltaTime() const
	{
		return (std::chrono::steady_clock::now() - this->tp).count() * 1e-9;
	}
	
	double Stopwatch::DeltaTime(double prefix) const
	{
		return (std::chrono::steady_clock::now() - this->tp).count() * 1e-9 / prefix;
	}
}