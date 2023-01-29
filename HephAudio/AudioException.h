#pragma once
#include "framework.h"
#include "StringBuffer.h"
#include <string>

namespace HephAudio
{
	struct AudioException final
	{
	private:
		int32_t errorCode;
		StringBuffer method; // Method name that exception has occurred.
		StringBuffer message;
		mutable StringBuffer errorString;
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