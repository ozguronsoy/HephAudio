#pragma once
#include "HephCommonFramework.h"
#include "StringBuffer.h"
#include "Event.h"
#include <vector>
#include <cinttypes>

#define RAISE_HEPH_EXCEPTION(sender, ex) ex.Raise(sender)
#define RAISE_AND_THROW_HEPH_EXCEPTION(sender, ex) ex.Raise(sender); throw ex

namespace HephCommon
{
	struct HephException final
	{
	private:
		static std::vector<HephException> exceptions;
	public:
		static HEPH_CONSTEXPR int64_t ec_none = 0;
		static HEPH_CONSTEXPR int64_t ec_fail = -1;
		static HEPH_CONSTEXPR int64_t ec_invalid_argument = -2;
		static HEPH_CONSTEXPR int64_t ec_not_implemented = -3;
		static HEPH_CONSTEXPR int64_t ec_insufficient_memory = -4;
		static HEPH_CONSTEXPR int64_t ec_not_found = -5;
	public:
		static Event OnException;
	public:
		int64_t errorCode;
		StringBuffer method;
		StringBuffer message;
		HephException();
		HephException(int64_t errorCode, StringBuffer method, StringBuffer message);
		void Raise(const void* pSender) const noexcept;
	public:
		static const HephException& LastException();
		static const std::vector<HephException>& AllExceptions();
	};

	struct HephExceptionEventArgs : public EventArgs
	{
		HephException exception;
		HephExceptionEventArgs(const void* pSender, HephException ex) : EventArgs(pSender), exception(ex) {}
		virtual ~HephExceptionEventArgs() = default;
	};
}