#pragma once
#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/**
	 * @brief raised when an operation takes too long to complete and reaches timeout.
	 *
	 */
	class HEPH_API TimeoutException : public Exception
	{
	public:
		/** @copydoc default_constructor */
		TimeoutException();

		/** @copydoc Exception(const std::string&,const std::string&) */
		TimeoutException(const std::string& method, const std::string& message);

		virtual std::string GetName() const override;

	protected:
		virtual void AddToExceptions() const override;
	};
}