#include "Stopwatch.h"

namespace HephCommon
{
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

	Stopwatch& Stopwatch::GetInstance() noexcept
	{
		// had to use singleton to dllexport the thread_local instance.
		static thread_local Stopwatch instance;
		return instance;
	}
}
