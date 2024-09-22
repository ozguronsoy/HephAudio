#include "Exceptions/InvalidOperationException.h"

namespace Heph
{
	InvalidOperationException::InvalidOperationException() : Exception() {}

	InvalidOperationException::InvalidOperationException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string InvalidOperationException::GetName() const
	{
		return "InvalidOperationException";
	}

	void InvalidOperationException::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_unique<InvalidOperationException>(*this));
	}
}