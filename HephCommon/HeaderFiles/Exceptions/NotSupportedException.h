#pragma once
#include "HephShared.h"
#include "Exception.h"

namespace Heph
{
	/**
	 * @brief raised when a feature is not supported on the current platform, environment, class, or method.
	 *
	 */
	class HEPH_API NotSupportedException : public Exception
	{
	public:
		/** @copydoc default_constructor */
		NotSupportedException();

		/** @copydoc Exception(const std::string&,const std::string&) */
		NotSupportedException(const std::string& method, const std::string& message);

		virtual std::string GetName() const override;

	protected:
		virtual void AddToExceptions() const override;
	};
}