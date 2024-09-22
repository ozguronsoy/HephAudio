#include "Exceptions/ExternalException.h"

namespace Heph
{
	ExternalException::ExternalException() : Exception() { }

	ExternalException::ExternalException(const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage)
		: Exception(method, message), externalSource(externalSource), externalMessage(externalMessage) {}

	std::string ExternalException::GetName() const
	{
		return "ExternalException";
	}

	void ExternalException::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_unique<ExternalException>(*this));
	}
}