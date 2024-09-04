#pragma once
#include "HephCommonShared.h"
#include "Event.h"
#include <string>
#include <cinttypes>
#include <vector>

#define HEPH_EC_OK						((int64_t)0)
#define HEPH_EC_FAIL					((int64_t)-1)
#define HEPH_EC_INVALID_ARGUMENT		((int64_t)-2)
#define HEPH_EC_NOT_IMPLEMENTED			((int64_t)-3)
#define HEPH_EC_INSUFFICIENT_MEMORY		((int64_t)-4)
#define HEPH_EC_NOT_FOUND				((int64_t)-5)
#define HEPH_EC_INVALID_OPERATION		((int64_t)-6)
#define HEPH_EC_TIMEOUT					((int64_t)-7)
#define HEPH_EC_NOT_SUPPORTED			((int64_t)-8)
#define HEPH_EC_EXTERNAL				((int64_t)INT64_MIN)

#define HEPH_EXCEPTION_DEFAULT_HANDLER	&HephCommon::HephException::DefaultHandler

#define RAISE_HEPH_EXCEPTION(sender, ex) (ex).Raise((const void*)(sender))
#define RAISE_AND_THROW_HEPH_EXCEPTION(sender, ex)	{																	\
														const HephCommon::HephException __temp_ex__ = (ex);				\
														__temp_ex__.Raise((const void*)(sender));						\
														throw __temp_ex__;												\
													}

namespace HephCommon
{
	struct HephException final
	{
		static thread_local std::vector<HephException> Exceptions;
		static inline Event OnException = Event();

		int64_t errorCode;
		int64_t externalErrorCode;
		std::string method;
		std::string message;
		std::string externalSource;
		std::string externalMessage;

		HephException();
		HephException(int64_t errorCode, const std::string& method, const std::string& message);
		HephException(int64_t externalErrorCode, const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage);

		void Raise(const void* pSender) const;

		static std::string ErrorCodeToString(int64_t errorCode);
		static void DefaultHandler(const EventParams& params);
	};

	struct HephExceptionEventArgs : public EventArgs
	{
		const void* pSender;
		HephException exception;
		HephExceptionEventArgs(const void* pSender, HephException ex) : pSender(pSender), exception(ex) {}
		virtual ~HephExceptionEventArgs() = default;
	};
}