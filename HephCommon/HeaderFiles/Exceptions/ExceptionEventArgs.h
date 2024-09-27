#pragma once
#include "HephShared.h"
#include "EventArgs.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/**
	 * @brief struct for storing the arguments for the audio exception events.
	 *
	 */
	struct HEPH_API ExceptionEventArgs : public EventArgs
	{
		/**
		 * pointer to the object that caused the exception.
		 *
		 */
		const void* pSender;

		/**
		 * the exception that's being raised.
		 *
		 */
		const Exception& exception;

		/**
		 * @copydoc constructor
		 *
		 * @param pSender @copydetails pSender
		 * @param ex @copydetails exception
		 */
		ExceptionEventArgs(const void* pSender, const Exception& ex);

		/** @copydoc destructor */
		virtual ~ExceptionEventArgs() = default;
	};
}