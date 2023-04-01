#pragma once
#include "framework.h"
#include "StringBuffer.h"
#include <cinttypes>

namespace HephCommon
{
	/// <summary>
	/// Holds the following information about an exception; the error code, the name of the method where it occurred, and the error message.
	/// </summary>
	struct HephException final
	{
	private:
		mutable StringBuffer resultMessage; // formatted string to display the exception information
	public:
		/// <summary>
		/// The error code.
		/// </summary>
		int32_t errorCode;
		/// <summary>
		/// The name of the method where the exception occurred.
		/// </summary>
		StringBuffer method;
		/// <summary>
		/// The error message.
		/// </summary>
		StringBuffer message;
		/// <summary>
		/// Creates and initializes an HephException instance with default values.
		/// </summary>
		HephException();
		/// <summary>
		/// Creates and initializes an HephException instance with the provided information.
		/// </summary>
		/// <param name="errorCode">The code of the error.</param>
		/// <param name="method">The name of the method where the exception occurred.</param>
		/// <param name="message">The error message.</param>
		HephException(int32_t errorCode, StringBuffer method, StringBuffer message);
		/// <summary>
		/// Combines the error information in an ASCII string.
		/// </summary>
		operator char* () const;
		/// <summary>
		/// Combines the error information in a wide string.
		/// </summary>
		operator wchar_t* () const;
		/// <summary>
		/// Combines the error information in a string.
		/// </summary>
		/// <param name="stringType">The character type used in the string.</param>
		/// <returns>A detailed error message.</returns>
		StringBuffer ToString(StringType stringType) const;
	};
}