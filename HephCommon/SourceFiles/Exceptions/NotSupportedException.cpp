#include "Exceptions/NotSupportedException.h"

namespace Heph
{
	NotSupportedException::NotSupportedException() : Exception() {}

	NotSupportedException::NotSupportedException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string NotSupportedException::GetName() const
	{
		return "NotSupportedException";
	}

	void NotSupportedException::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_shared<NotSupportedException>(*this));
	}
}