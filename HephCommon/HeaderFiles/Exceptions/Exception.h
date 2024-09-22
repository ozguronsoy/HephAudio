#pragma once
#include "HephShared.h"
#include "Event.h"
#include <exception>
#include <string>
#include <vector>
#include <memory>

/** @file */

/**
 * default handler for the \link Exception::OnException OnException \endlink event.
 *
 */
#define HEPH_EXCEPTION_DEFAULT_HANDLER	&Heph::Exception::DefaultHandler

 /**
  * raises the \a ex but does not throw it.
  *
  */
#define HEPH_RAISE_EXCEPTION(pSender, ex) (ex).Raise((const void*)(pSender))

  /**
   * raises and throws the \a ex.
   *
   */
#define HEPH_RAISE_AND_THROW_EXCEPTION(pSender, ex)	{																	\
														const Heph::Exception __temp_ex__ = (ex);						\
														__temp_ex__.Raise((const void*)(pSender));						\
														throw __temp_ex__;												\
													}

namespace Heph
{
	/**
	 * @brief stores exception information.
	 * Base class for exceptions.
	 *
	 */
	class HEPH_API Exception : public std::exception
	{
	public:
		/**
		 * raised when an exception occurs.
		 *
		 */
		static inline Event OnException = Event();

	protected:
		/**
		 * name of the method where the exception occurred.
		 *
		 */
		std::string method;

	public:
		/** @copydoc default_constructor */
		Exception();

		/**
		 * @copydoc constructor
		 *
		 * @param method @copydetails method
		 * @param message description of the exception.
		 */
		Exception(const std::string& method, const std::string& message);

		/** @copydoc destructor */
		virtual ~Exception() = default;

		/**
		 * gets the human readable name of the exception.
		 *
		 */
		virtual std::string GetName() const;

		/**
		 * gets the method.
		 *
		 */
		virtual const std::string& GetMethod() const;

		/**
		 * gets the message. 
		 * 
		 */
		virtual std::string GetMessage() const;

		/**
		 * raises the \link Heph::Exception::OnException OnException \endlink event.
		 *
		 * @param pSender pointer to the object instance that caused the exception.
		 */
		virtual void Raise(const void* pSender) const;

	protected:
		/**
		 * adds the exception to the thread local exceptions vector.
		 *
		 */
		virtual void AddToExceptions() const;

	public:
		/**
		 * gets the exceptions that occurred in the current thread.
		 *
		 */
		static std::vector<std::unique_ptr<Exception>>& GetExceptions() noexcept;

		/**
		 * the default handler for the \link Heph::Exception::OnException OnException \endlink event.
		 *
		 *
		 */
		static void DefaultHandler(const EventParams& params);
	};
}