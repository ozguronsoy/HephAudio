#include "HephException.h"

namespace HephCommon
{
	thread_local std::vector<HephException> HephException::Exceptions = std::vector<HephException>();

	HephException::HephException() : errorCode(HEPH_EC_OK), externalErrorCode(HEPH_EC_OK) { }

	HephException::HephException(int64_t errorCode, const std::string& method, const std::string& message)
		: errorCode(errorCode), externalErrorCode(HEPH_EC_OK), 
		method(method), message(message) {}

	HephException::HephException(int64_t externalErrorCode, const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage)
		: errorCode(HEPH_EC_EXTERNAL), externalErrorCode(externalErrorCode),
		method(method), message(message), 
		externalSource(externalSource), externalMessage(externalMessage) {}

	void HephException::Raise(const void* pSender) const
	{
		HephExceptionEventArgs args(pSender, *this);
		HephException::OnException.Invoke(&args, nullptr);
		HephException::Exceptions.push_back(*this);
	}

	std::string HephException::ErrorCodeToString(int64_t errorCode)
	{
		switch (errorCode)
		{
		case HEPH_EC_OK:
			return "ok";
		case HEPH_EC_FAIL:
			return "fail";
		case HEPH_EC_INVALID_ARGUMENT:
			return "invalid argument";
		case HEPH_EC_NOT_IMPLEMENTED:
			return "not implemented";
		case HEPH_EC_INSUFFICIENT_MEMORY:
			return "insufficient memory";
		case HEPH_EC_NOT_FOUND:
			return "not found";
		case HEPH_EC_INVALID_OPERATION:
			return "invalid operation";
		case HEPH_EC_TIMEOUT:
			return "timeout";
		case HEPH_EC_EXTERNAL:
			return "external error";
		default:
			return "unknown error";
		}
	}
}