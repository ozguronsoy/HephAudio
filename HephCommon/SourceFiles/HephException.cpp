#include "HephException.h"

namespace HephCommon
{
	thread_local std::vector<HephException> HephException::Exceptions = std::vector<HephException>();
	Event HephException::OnException = Event();

	HephException::HephException() : errorCode(HEPH_EC_NONE), method(""), message(""), externalSource(""), externalMessage("") { }

	HephException::HephException(int64_t errorCode, const std::string& method, const std::string& message)
		: errorCode(errorCode), method(method), message(message), externalSource(""), externalMessage("") {}

	HephException::HephException(int64_t errorCode, const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage)
		: errorCode(errorCode), method(method), message(message)
		, externalSource(externalSource), externalMessage(externalMessage) {}

	void HephException::Raise(const void* pSender) const
	{
		HephExceptionEventArgs args(pSender, *this);
		HephException::OnException.Invoke(&args, nullptr);
		HephException::Exceptions.push_back(*this);
	}
}