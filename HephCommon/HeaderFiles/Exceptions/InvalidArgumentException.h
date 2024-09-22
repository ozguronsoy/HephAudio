#pragma once
#include "HephShared.h"
#include "Exception.h"

namespace Heph
{
	/**
	 * @brief raised when an argument passed to a method is invalid.
	 *
	 */
	class HEPH_API InvalidArgumentException : public Exception
	{
	public:
		/** @copydoc default_constructor */
		InvalidArgumentException();

		/** @copydoc Exception(const std::string&,const std::string&) */
		InvalidArgumentException(const std::string& method, const std::string& message);

		virtual std::string GetName() const override;

	protected:
		virtual void AddToExceptions() const override;
	};
}