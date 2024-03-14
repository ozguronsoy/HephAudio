#include "HephException.h"

namespace HephCommon
{
	std::unordered_map<std::thread::id, std::vector<HephException>> HephException::threadIdToExceptionsMap = std::unordered_map<std::thread::id, std::vector<HephException>>();
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
		HephException::threadIdToExceptionsMap[std::this_thread::get_id()].push_back(*this);
	}
	const HephException& HephException::LastException()
	{
		const std::vector<HephException>& exceptions = HephException::threadIdToExceptionsMap[std::this_thread::get_id()];
		return exceptions.size() > 0 ? exceptions[exceptions.size() - 1] : HephException();
	}
	const HephException& HephException::GetException(size_t index)
	{
		const std::vector<HephException>& exceptions = HephException::threadIdToExceptionsMap[std::this_thread::get_id()];
		return exceptions.size() > index ? exceptions[index] : HephException();
	}
	size_t HephException::GetExceptionCount()
	{
		const std::vector<HephException>& exceptions = HephException::threadIdToExceptionsMap[std::this_thread::get_id()];
		return exceptions.size();
	}
}