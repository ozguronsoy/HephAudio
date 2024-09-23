#include "Exceptions/NotFoundException.h"

namespace Heph
{
	NotFoundException::NotFoundException() : Exception() {}

	NotFoundException::NotFoundException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string NotFoundException::GetName() const
	{
		return "NotFoundException";
	}

	void NotFoundException::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_shared<NotFoundException>(*this));
	}
}