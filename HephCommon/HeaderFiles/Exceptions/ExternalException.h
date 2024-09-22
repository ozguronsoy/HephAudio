#pragma once
#include "HephShared.h"
#include "Exception.h"

namespace Heph
{
	/**
	 * @brief raised when an operation from an external library/API fails.
	 *
	 */
	class HEPH_API ExternalException : public Exception
	{
	public:
		/**
		 * name of the external source that caused the exception.
		 *
		 */
		std::string externalSource;

		/**
		 * description of the exception provided by the external source.
		 * 
		 */
		std::string externalMessage;

	public:
		/** @copydoc default_constructor */
		ExternalException();

		/** 
		 * @copydoc Exception(const std::string&,const std::string&) 
		 * 
		 * @param externalSource @copydetails externalSource
		 * @param externalMessage @copydetails externalMessage
		 */
		ExternalException(const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage);

		virtual std::string GetName() const override;

	protected:
		virtual void AddToExceptions() const override;
	};
}