#pragma once
#include "HephShared.h"

/** @file */

namespace Heph
{
	/**
	 * @brief base class for storing the results of an \link Heph::Event event \endlink.
	 * 
	 */
	struct HEPH_API EventResult
	{
		/**
		 * indicates the event is handled and no further handlers will be invoked.<br>
		 * set by the user.
		 */
		bool isHandled;

		/** @copydoc default_constructor */
		EventResult();

		/** @copydoc destructor */
		virtual ~EventResult() = default;
	};
}