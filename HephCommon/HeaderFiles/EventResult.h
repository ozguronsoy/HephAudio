#pragma once

namespace HephCommon
{
	/**
	 * @brief base class for storing the results of an \link HephCommon::Event event \endlink.
	 * 
	 */
	struct EventResult
	{
		/**
		 * indicates the event is handled and no further handlers will be invoked.<br>
		 * set by the user.
		 */
		bool isHandled;
		EventResult() : isHandled(false) {}
		virtual ~EventResult() = default;
	};
}