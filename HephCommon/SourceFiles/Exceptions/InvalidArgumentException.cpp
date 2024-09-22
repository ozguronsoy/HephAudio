#include "Exceptions/InvalidArgumentException.h"

namespace Heph
{
	InvalidArgumentException::InvalidArgumentException() : Exception() {}

	InvalidArgumentException::InvalidArgumentException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string InvalidArgumentException::GetName() const
	{
		return "InvalidArgumentException";
	}

	void InvalidArgumentException::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_unique<InvalidArgumentException>(*this));
	}
}