#pragma once
#include "HephShared.h"
#include "Exception.h"

namespace Heph
{
	/**
	 * @brief raised when a search fails.
	 *
	 */
	class HEPH_API NotFoundException : public Exception
	{
	public:
		/** @copydoc default_constructor */
		NotFoundException();

		/** @copydoc Exception(const std::string&,const std::string&) */
		NotFoundException(const std::string& method, const std::string& message);

		virtual std::string GetName() const override;

	protected:
		virtual void AddToExceptions() const override;
	};
}