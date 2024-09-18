#pragma once
#include "HephCommonShared.h"
#include <chrono>

/** @file */

#define HEPH_SW_NANO	(1e-9)
#define HEPH_SW_MICRO	(1e-6)
#define HEPH_SW_MILLI	(1e-3)

/**
 * resets the thread local Stopwatch instance.
 * 
 */
#define HEPH_SW_RESET	HephCommon::Stopwatch::Instance.Reset()

/**
 * gets the elapsed time since the last reset of the thread local Stopwatch instance in seconds.
 * 
 * 	
 */
#define HEPH_SW_DT		HephCommon::Stopwatch::Instance.DeltaTime()

/**
 * gets the elapsed time since the last reset of the thread local Stopwatch instance in seconds.
 * 
 */
#define HEPH_SW_DT_S	HephCommon::Stopwatch::Instance.DeltaTime()

/**
 * gets the elapsed time since the last reset of the thread local Stopwatch instance in milliseconds.
 * 
 */
#define HEPH_SW_DT_MS	HephCommon::Stopwatch::Instance.DeltaTime(HEPH_SW_MILLI)

/**
 * gets the elapsed time since the last reset of the thread local Stopwatch instance in microseconds.
 * 
 */
#define HEPH_SW_DT_US	HephCommon::Stopwatch::Instance.DeltaTime(HEPH_SW_MICRO)

/**
 * gets the elapsed time since the last reset of the thread local Stopwatch instance in nanoseconds.
 * 
 */
#define HEPH_SW_DT_NS	HephCommon::Stopwatch::Instance.DeltaTime(HEPH_SW_NANO)

namespace HephCommon
{
	/**
	 * @brief class for measuring time.
	 * 
	 */
	class Stopwatch final
	{
	private:
		std::chrono::steady_clock::time_point tp;

	public:
		/**
		 * @brief a static instance that is pinned to the current thread.
		 * 
		 */
		static thread_local Stopwatch Instance;
		
	public:
		/** @copydoc default_constructor */
		Stopwatch();

		/**
		 * restarts the stopwatch.
		 * 
		 */
		void Reset();

		/**
		 * gets the elapsed time since the last reset in seconds.
		 * 
		 */
		double DeltaTime() const;

		/**
		 * gets the elapsed time since the last reset.
		 * 
		 * @param prefix the desired metric prefix.
		 */
		double DeltaTime(double prefix) const;
	};
}
