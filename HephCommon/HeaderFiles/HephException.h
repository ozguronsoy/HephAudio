#pragma once
#include "HephCommonShared.h"
#include "Event.h"
#include <string>
#include <cinttypes>
#include <vector>

/** @file */

#define HEPH_EC_OK						((int64_t)0)
#define HEPH_EC_FAIL					((int64_t)-1)
#define HEPH_EC_INVALID_ARGUMENT		((int64_t)-2)
#define HEPH_EC_NOT_IMPLEMENTED			((int64_t)-3)
#define HEPH_EC_INSUFFICIENT_MEMORY		((int64_t)-4)
#define HEPH_EC_NOT_FOUND				((int64_t)-5)
#define HEPH_EC_INVALID_OPERATION		((int64_t)-6)
#define HEPH_EC_TIMEOUT					((int64_t)-7)
#define HEPH_EC_NOT_SUPPORTED			((int64_t)-8)

/**
 * default handler for the \link HephCommon::HephException::OnException OnException \endlink event.
 * 
 */
#define HEPH_EXCEPTION_DEFAULT_HANDLER	&HephCommon::HephException::DefaultHandler

/**
 * raises the \a ex but does not throw it.
 * 
 */
#define RAISE_HEPH_EXCEPTION(pSender, ex) (ex).Raise((const void*)(pSender))

/**
 * raises and throws the \a ex.
 * 
 */
#define RAISE_AND_THROW_HEPH_EXCEPTION(pSender, ex)	{																	\
														const HephCommon::HephException __temp_ex__ = (ex);				\
														__temp_ex__.Raise((const void*)(pSender));						\
														throw __temp_ex__;												\
													}

namespace HephCommon
{
	/**
	 * @brief stores information about an exception.
	 * 
	 */
	struct HephException final
	{
		/**
		 * a vector of exceptions local to the current thread.
		 * 
		 */
		static thread_local std::vector<HephException> Exceptions;

		/**
		 * raised when an exception occurs.
		 * 
		 */
		static inline Event OnException = Event();

		/**
		 * either one of the <b>HEPH_EC_*</b> codes or an error code from an external source.
		 * 
		 */
		int64_t errorCode;

		/**
		 * name of the method where the exception occurred.
		 * 
		 */
		std::string method;

		/**
		 * description of the exception.
		 * 
		 */
		std::string message;

		/**
		 * name of the external source that caused the exception.
		 * 
		 */
		std::string externalSource;

		/**
		 * description of the exception provided by the external source.
		 * 
		 */
		std::string externalMessage;

		HephException();
		HephException(int64_t errorCode, const std::string& method, const std::string& message);
		HephException(int64_t errorCode, const std::string& method, const std::string& message, const std::string& externalSource, const std::string& externalMessage);

		/**
		 * raises the \link HephCommon::HephException::OnException OnException \endlink event.
		 * 
		 * @param pSender pointer to the object instance that caused the exception.
		 */
		void Raise(const void* pSender) const;

		/**
		 * converts <b>HEPH_EC_*</b> codes to string.
		 * 
		 */
		static std::string ErrorCodeToString(int64_t errorCode);

		/**
		 * the default handler for the \link HephCommon::HephException::OnException OnException \endlink event.
		 * 
		 *
		 */
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