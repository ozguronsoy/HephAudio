#pragma once
#include "HephCommonFramework.h"
#include "StringBuffer.h"
#include "Event.h"
#include <vector>
#include <cinttypes>

#define HEPH_EC_NONE 0
#define HEPH_EC_FAIL -1
#define HEPH_EC_INVALID_ARGUMENT -2
#define HEPH_EC_NOT_IMPLEMENTED -3
#define HEPH_EC_INSUFFICIENT_MEMORY -4
#define HEPH_EC_NOT_FOUND -5

#define RAISE_HEPH_EXCEPTION(sender, ex) ex.Raise(sender)
#define RAISE_AND_THROW_HEPH_EXCEPTION(sender, ex) RAISE_HEPH_EXCEPTION(sender, ex); throw ex

namespace HephCommon
{
	struct HephException final
	{
	private:
		static std::vector<HephException> exceptions;
	public:
		static Event OnException;
	public:
		int64_t errorCode;
		StringBuffer method;
		StringBuffer message;
		HephException();
		HephException(int64_t errorCode, StringBuffer method, StringBuffer message);
		void Raise(const void* pSender) const;
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