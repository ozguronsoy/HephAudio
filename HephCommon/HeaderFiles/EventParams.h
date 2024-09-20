#pragma once
#include "HephCommonShared.h"
#include "EventArgs.h"
#include "EventResult.h"
#include "UserEventArgs.h"

/** @file */

namespace HephCommon
{
	/**
	 * @brief stores the information required to handle an event.
	 * 
	 */
	struct HEPH_API EventParams final
	{
		/**
		 * pointer to the arguments, can be null.
		 * 
		 */
		EventArgs* pArgs;

		/**
		 * pointer to the result, can be null.
		 * 
		 */
		EventResult* pResult;

		/**
		 * contains the extra arguments passed by the user.
		 * 
		 */
		const UserEventArgs& userEventArgs;
		
		/**
		 * @copydoc constructor
		 * 
		 * @param userEventArgs @copydetails userEventArgs 
		 */
		EventParams(const UserEventArgs& userEventArgs) : pArgs(nullptr), pResult(nullptr), userEventArgs(userEventArgs) {}
	};
}