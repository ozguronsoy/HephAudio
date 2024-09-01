#pragma once
#include "HephCommonShared.h"
#include "Event.h"
#include <string>
#include <cinttypes>
#include <vector>
#include <thread>

#define HEPH_EC_NONE (0)
#define HEPH_EC_FAIL (-1)
#define HEPH_EC_INVALID_ARGUMENT (-2)
#define HEPH_EC_NOT_IMPLEMENTED (-3)
#define HEPH_EC_INSUFFICIENT_MEMORY (-4)
#define HEPH_EC_NOT_FOUND (-5)
#define HEPH_EC_INVALID_OPERATION (-6)

#define RAISE_HEPH_EXCEPTION(sender, ex) ex.Raise(sender)
#define RAISE_AND_THROW_HEPH_EXCEPTION(sender, ex) RAISE_HEPH_EXCEPTION(sender, ex); throw ex

namespace HephCommon
{
	struct HephException final
	{
	public:
		static thread_local std::vector<HephException> Exceptions;
		static inline Event OnException = Event();
	public:
		int64_t errorCode;
		std::string method;
		std::string message;
		std::string externalSource;
		std::string externalMessage;
		HephException();
		HephException(int64_t errorCode, const std::string& method, const std::string& message);
		HephException(int64_t errorCode, const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage);
		void Raise(const void* pSender) const;
	};

	struct HephExceptionEventArgs : public EventArgs
	{
		const void* pSender;
		HephException exception;
		HephExceptionEventArgs(const void* pSender, HephException ex) : pSender(pSender), exception(ex) {}
		virtual ~HephExceptionEventArgs() = default;
	};
}