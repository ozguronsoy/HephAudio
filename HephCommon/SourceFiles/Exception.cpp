#include "Exception.h"
#include "ConsoleLogger.h"
#include "StringHelpers.h"

namespace Heph
{
	Exception::Exception() : errorCode(HEPH_EC_OK) { }

	Exception::Exception(int64_t errorCode, const std::string& method, const std::string& message)
		: errorCode(errorCode), method(method), message(message) {}

	Exception::Exception(int64_t errorCode, const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage)
		: errorCode(errorCode), method(method), message(message),
		externalSource(externalSource), externalMessage(externalMessage) {}

	void Exception::Raise(const void* pSender) const
	{
		ExceptionEventArgs args(pSender, *this);
		Exception::OnException.Invoke(&args, nullptr);
		Exception::GetExceptions().push_back(*this);
	}

	std::string Exception::ErrorCodeToString(int64_t errorCode)
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
		case HEPH_EC_NOT_SUPPORTED:
			return "not supported";
		default:
			return "unknown error";
		}
	}

	void Exception::DefaultHandler(const EventParams& params)
	{
#if defined(__ANDROID__)
		const std::string x = "";
#else
		const std::string x = "\n";
#endif

		const Exception& ex = ((ExceptionEventArgs*)params.pArgs)->exception;
		if (ex.externalSource != "" && ex.externalMessage != "")
		{
			ConsoleLogger::LogError(x +
				"error code:\t\t" + StringHelpers::ToHexString(ex.errorCode) +
				"\nmethod:\t\t\t" + ex.method + "\nmessage:\t\t" + ex.message +
				"\nexternal source:\t" + ex.externalSource + "\nexternal message:\t" + ex.externalMessage + "\n"
			);
		}
		else
		{
			ConsoleLogger::LogError(x +
				"error code:\t" + StringHelpers::ToString(ex.errorCode) +
				"\nmethod:\t\t" + ex.method + "\nmessage:\t" + ex.message + "\n"
			);
		}
	}

	std::vector<Exception>& Exception::GetExceptions() noexcept
	{
		// had to use singleton to dllexport the thread_local instance.
		static thread_local std::vector<Exception> instance;
		return instance;
	}

	ExceptionEventArgs::ExceptionEventArgs(const void* pSender, const Exception& ex) : pSender(pSender), exception(ex) {}
}