#include "gtest/gtest.h"
#include "Stopwatch.h"
#include <thread>
#include <chrono>

TEST(StopwatchTest, All)
{
	constexpr double sleepDuration_ms = 100;

	for (size_t i = 0; i < 2; ++i)
	{
		HEPH_SW_RESET;
		std::this_thread::sleep_for(std::chrono::milliseconds((uint64_t)sleepDuration_ms));

		EXPECT_GE(HEPH_SW_DT, sleepDuration_ms * 1e-3);
		EXPECT_GE(HEPH_SW_DT_S, sleepDuration_ms * 1e-3);
		EXPECT_GE(HEPH_SW_DT_MS, sleepDuration_ms);
		EXPECT_GE(HEPH_SW_DT_US, sleepDuration_ms * 1e3);
		EXPECT_GE(HEPH_SW_DT_NS, sleepDuration_ms * 1e6);
	}
}