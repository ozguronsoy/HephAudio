#include "Exceptions/InsufficientMemoryException.h"

namespace Heph
{
	InsufficientMemoryException::InsufficientMemoryException() : Exception() {}

	InsufficientMemoryException::InsufficientMemoryException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string InsufficientMemoryException::GetName() const
	{
		return "InsufficientMemoryException";
	}

	void InsufficientMemoryException::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_shared<InsufficientMemoryException>(*this));
	}
}