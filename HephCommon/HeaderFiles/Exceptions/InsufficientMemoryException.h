#pragma once
#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/**
	 * @brief raised when an allocation fails due to insufficient memory.
	 *
	 */
	class HEPH_API InsufficientMemoryException : public Exception
	{
	public:
		/** @copydoc default_constructor */
		InsufficientMemoryException();

		/** @copydoc Exception(const std::string&,const std::string&) */
		InsufficientMemoryException(const std::string& method, const std::string& message);

		virtual std::string GetName() const override;
	
	protected:
		virtual void AddToExceptions() const override;
	};
}