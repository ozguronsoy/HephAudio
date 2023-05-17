#pragma once
#include "HephCommonFramework.h"
#include "StringBuffer.h"
#include "Event.h"
#include <cinttypes>

#define RAISE_HEPH_EXCEPTION(sender, ex) { HephCommon::HephExceptionEventArgs args = HephCommon::HephExceptionEventArgs(sender, ex); HephCommon::HephException::OnException.Invoke(&args, nullptr);}
#define RAISE_AND_THROW_HEPH_EXCEPTION(sender, ex) { HephCommon::HephExceptionEventArgs args = HephCommon::HephExceptionEventArgs(sender, ex); HephCommon::HephException::OnException.Invoke(&args, nullptr); throw ex;}

namespace HephCommon
{
	struct HephException final
	{
	public:
		static constexpr int64_t ec_none = 0;
		static constexpr int64_t ec_fail = -1;
		static constexpr int64_t ec_invalid_argument = -2;
		static constexpr int64_t ec_not_implemented = -3;
		static constexpr int64_t ec_insufficient_memory = -4;
		static constexpr int64_t ec_not_found = -5;
	public:
		static Event OnException;
	public:
		int64_t errorCode;
		StringBuffer method;
		StringBuffer message;
		HephException();
		HephException(int64_t errorCode, StringBuffer method, StringBuffer message);
	};

	struct HephExceptionEventArgs : public EventArgs
	{
		HephException exception;
		HephExceptionEventArgs(const void* pSender, HephException ex) : EventArgs(pSender), exception(ex) {}
		virtual ~HephExceptionEventArgs() = default;
	};
}