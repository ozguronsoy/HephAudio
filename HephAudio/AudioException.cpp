#include "AudioException.h"
#include <exception>
#include <sstream>
#include <iomanip>

namespace HephAudio
{
	AudioException::AudioException()
	{
		this->errorCode = 0;
		this->method = L"";
		this->message = L"";
		this->errorString = "";
		this->errorWString = L"";
	}
	AudioException::AudioException(int32_t errorCode, const wchar_t* method, const wchar_t* message)
	{
		this->errorCode = errorCode;
		this->method = method;
		this->message = message;
		this->errorString = "";
		this->errorWString = L"";
	}
	const char* AudioException::ToString() const
	{
		this->errorString = "Audio Exception " + this->ErrorCodeToHex() + " (" + std::to_string(this->errorCode) + ")\nMethod: " + std::string(this->method.begin(), this->method.end()) + "\nMessage: " + std::string(this->message.begin(), this->message.end());
		return errorString.c_str();
	}
	const wchar_t* AudioException::ToWString() const
	{
		this->errorWString = L"Audio Exception " + this->ErrorCodeToHexW() + L" (" + std::to_wstring(this->errorCode) + L")\nMethod: " + this->method + L"\nMessage: " + this->message;
		return this->errorWString.c_str();
	}
	std::string AudioException::ErrorCodeToHex() const
	{
		std::stringstream ss;
		ss << "0x" << std::setfill('0') << std::setw(sizeof(int64_t)) << std::hex << this->errorCode;
		return ss.str();
	}
	std::wstring AudioException::ErrorCodeToHexW() const
	{
		std::wstringstream ss;
		ss << L"0x" << std::setfill(L'0') << std::setw(sizeof(int64_t)) << std::hex << this->errorCode;
		return ss.str();
	}
}