#pragma once
#include "framework.h"
#include <string>

namespace HephAudio
{
	struct AudioException final
	{
	private:
		int32_t errorCode;
		std::wstring method; // Method name that exception has occurred.
		std::wstring message;
		mutable std::string errorString;
		mutable std::wstring errorWString;
	public:
		AudioException();
		AudioException(int32_t errorCode, const wchar_t* method, const wchar_t* message);
		const char* ToString() const;
		const wchar_t* ToWString() const;
	private:
		std::string ErrorCodeToHex() const;
		std::wstring ErrorCodeToHexW() const;
	};
}