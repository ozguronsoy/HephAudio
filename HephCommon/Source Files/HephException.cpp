#include "HephException.h"

namespace HephCommon
{
	std::vector<HephException> HephException::exceptions = std::vector<HephException>(0);
	Event HephException::OnException = Event();
	HephException::HephException() : errorCode(HephException::ec_none), method(""), message("") { }
	HephException::HephException(int64_t errorCode, StringBuffer method, StringBuffer message) : errorCode(errorCode), method(method), message(message) {}
	void HephException::Raise(const void* pSender) const noexcept
	{
		HephExceptionEventArgs args(pSender, *this);
		HephException::OnException.Invoke(&args, nullptr);
		HephException::exceptions.push_back(*this);
	}
	const HephException& HephException::LastException()
	{
		return HephException::exceptions[HephException::exceptions.size() - 1];
	}
	const std::vector<HephException>& HephException::AllExceptions()
	{
		return HephException::exceptions;
	}
}