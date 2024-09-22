#include "Exceptions/TimeoutException.h"

namespace Heph
{
	TimeoutException::TimeoutException() : Exception() {}

	TimeoutException::TimeoutException(const std::string& method, const std::string& message) : Exception(method, message) {}

	std::string TimeoutException::GetName() const
	{
		return "TimeoutException";
	}

	void TimeoutException::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_unique<TimeoutException>(*this));
	}
}