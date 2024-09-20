#pragma once
#include "HephCommonShared.h"

/** @file */

namespace HephCommon
{
	/**
	 * @brief base class for storing arguments for an \link HephCommon::Event event \endlink.
	 * 
	 */
	struct HEPH_API EventArgs
	{
		/** @copydoc destructor */
		virtual ~EventArgs() = default;
	};
}