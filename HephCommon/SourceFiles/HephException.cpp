#include "HephException.h"

namespace HephCommon
{
	std::unordered_map<std::thread::id, std::vector<HephException>> HephException::threadIdToExceptionsMap = std::unordered_map<std::thread::id, std::vector<HephException>>();
	HephException HephException::DefaultException = HephException(HEPH_EC_NONE, "Default", "Default");
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
		HephException::threadIdToExceptionsMap[std::this_thread::get_id()].push_back(*this);
	}
	const HephException& HephException::LastException()
	{
		const std::vector<HephException>& exceptions = HephException::threadIdToExceptionsMap[std::this_thread::get_id()];
		return exceptions.size() > 0 ? exceptions[exceptions.size() - 1] : HephException::DefaultException;
	}
	const HephException& HephException::GetException(size_t index)
	{
		const std::vector<HephException>& exceptions = HephException::threadIdToExceptionsMap[std::this_thread::get_id()];
		return exceptions.size() > index ? exceptions[index] : HephException::DefaultException;
	}
	size_t HephException::GetExceptionCount()
	{
		const std::vector<HephException>& exceptions = HephException::threadIdToExceptionsMap[std::this_thread::get_id()];
		return exceptions.size();
	}
}