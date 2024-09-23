#include "Exceptions/NotImplementedException.h"

namespace Heph
{
	NotImplementedException::NotImplementedException() : Exception() {}

	NotImplementedException::NotImplementedException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string NotImplementedException::GetName() const
	{
		return "NotImplementedException";
	}

	void NotImplementedException::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_shared<NotImplementedException>(*this));
	}
}