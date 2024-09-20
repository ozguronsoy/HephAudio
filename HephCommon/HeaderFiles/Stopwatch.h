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
#define HEPH_SW_RESET	HephCommon::Stopwatch::GetInstance().Reset()

 /**
  * gets the elapsed time since the last reset of the thread local Stopwatch instance in seconds.
  *
  *
  */
#define HEPH_SW_DT		HephCommon::Stopwatch::GetInstance().DeltaTime()

  /**
   * gets the elapsed time since the last reset of the thread local Stopwatch instance in seconds.
   *
   */
#define HEPH_SW_DT_S	HephCommon::Stopwatch::GetInstance().DeltaTime()

   /**
	* gets the elapsed time since the last reset of the thread local Stopwatch instance in milliseconds.
	*
	*/
#define HEPH_SW_DT_MS	HephCommon::Stopwatch::GetInstance().DeltaTime(HEPH_SW_MILLI)

	/**
	 * gets the elapsed time since the last reset of the thread local Stopwatch instance in microseconds.
	 *
	 */
#define HEPH_SW_DT_US	HephCommon::Stopwatch::GetInstance().DeltaTime(HEPH_SW_MICRO)

	 /**
	  * gets the elapsed time since the last reset of the thread local Stopwatch instance in nanoseconds.
	  *
	  */
#define HEPH_SW_DT_NS	HephCommon::Stopwatch::GetInstance().DeltaTime(HEPH_SW_NANO)

namespace HephCommon
{
	/**
	 * @brief class for measuring time.
	 *
	 */
	class HEPH_API Stopwatch final
	{
	private:
		std::chrono::steady_clock::time_point tp;
		
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

		/**
		 * @brief gets the instance that's local to the current thread.
		 *
		 */
		static Stopwatch& GetInstance() noexcept;
	};
}
