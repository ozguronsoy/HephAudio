#pragma once

namespace HephCommon
{
	/**
	 * @brief base class for storing arguments for an \link HephCommon::Event event \endlink.
	 * 
	 */
	struct EventArgs
	{
		virtual ~EventArgs() = default;
	};
}