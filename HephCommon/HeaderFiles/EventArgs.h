#pragma once
#include "HephShared.h"

/** @file */

namespace Heph
{
	/**
	 * @brief base class for storing arguments for an \link Heph::Event event \endlink.
	 * 
	 */
	struct HEPH_API EventArgs
	{
		/** @copydoc destructor */
		virtual ~EventArgs() = default;
	};
}