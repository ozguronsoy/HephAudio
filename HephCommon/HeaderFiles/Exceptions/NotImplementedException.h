#pragma once
#include "HephShared.h"
#include "Exception.h"

/** @file */

namespace Heph
{
	/**
	 * @brief raised when a method or a class is not fully implemented yet.
	 *
	 */
	class HEPH_API NotImplementedException : public Exception
	{
	public:
		/** @copydoc default_constructor */
		NotImplementedException();

		/** @copydoc Exception(const std::string&,const std::string&) */
		NotImplementedException(const std::string& method, const std::string& message);

		virtual std::string GetName() const override;

	protected:
		virtual void AddToExceptions() const override;
	};
}