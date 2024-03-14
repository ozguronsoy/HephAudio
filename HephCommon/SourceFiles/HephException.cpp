#include "HephException.h"

namespace HephCommon
{
	std::vector<HephException> HephException::exceptions = std::vector<HephException>(0);
	Event HephException::OnException = Event();
	
	HephException::HephException() : errorCode(HEPH_EC_NONE), method(nullptr), message(nullptr), externalSource(nullptr), externalMessage(nullptr) { }
	
	HephException::HephException(int64_t errorCode, StringBuffer method, StringBuffer message) 
		: errorCode(errorCode), method(method), message(message), externalSource(nullptr), externalMessage(nullptr) {}
	
	HephException::HephException(int64_t errorCode, StringBuffer method, StringBuffer message, StringBuffer externalSource, StringBuffer externalMessage)
		: errorCode(errorCode), method(method), message(message)
		, externalSource(externalSource), externalMessage(externalMessage) {}
	
	void HephException::Raise(const void* pSender) const
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