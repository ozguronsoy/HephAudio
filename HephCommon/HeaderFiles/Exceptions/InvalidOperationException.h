#pragma once
#include "HephShared.h"
#include "Exception.h"

namespace Heph
{
	/**
	 * @brief raised when an operation is invalid.
	 *
	 */
	class HEPH_API InvalidOperationException : public Exception
	{
	public:
		/** @copydoc default_constructor */
		InvalidOperationException();

		/** @copydoc Exception(const std::string&,const std::string&) */
		InvalidOperationException(const std::string& method, const std::string& message);

		virtual std::string GetName() const override;

	protected:
		virtual void AddToExceptions() const override;
	};
}