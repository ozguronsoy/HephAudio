#include "AudioException.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace HephAudio
{
	AudioException::AudioException()
	{
		this->errorCode = 0;
		this->method = L"";
		this->message = L"";
		this->errorString = L"";
	}
	AudioException::AudioException(int32_t errorCode, const wchar_t* method, const wchar_t* message)
	{
		this->errorCode = errorCode;
		this->method = method;
		this->message = message;
		this->errorString = L"";
	}
	const char* AudioException::ToString() const
	{
		StringBuffer tempMethod = StringBuffer(this->method, StringType::Normal);
		StringBuffer tempMessage = StringBuffer(this->message, StringType::Normal);
		this->errorString = ("Audio Exception " + this->ErrorCodeToHex() + " (" + std::to_string(this->errorCode) + ")\nMethod: " + tempMethod.c_str() + "\nMessage: " + tempMessage.c_str()).c_str();
		return this->errorString.c_str();
	}
	const wchar_t* AudioException::ToWString() const
	{
		this->errorString = (L"Audio Exception " + this->ErrorCodeToHexW() + L" (" + std::to_wstring(this->errorCode) + L")\nMethod: " + this->method.wc_str() + L"\nMessage: " + this->message.wc_str()).c_str();
		return this->errorString.wc_str();
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