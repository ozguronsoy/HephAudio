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
	}
	AudioException::AudioException(int32_t errorCode, StringBuffer&& method, StringBuffer&& message)
	{
		this->errorCode = errorCode;
		this->method = method;
		this->message = message;
	}
	AudioException::operator char* () const
	{
		return "Audio Exception " + this->ErrorCodeToHex() + " (" + std::to_string(this->errorCode).c_str() + ")\nMethod: " + this->method + "\nMessage: " + this->message;
	}
	AudioException::operator wchar_t* () const
	{
		return L"Audio Exception " + this->ErrorCodeToHex() + L" (" + std::to_wstring(this->errorCode).c_str() + L")\nMethod: " + this->method + L"\nMessage: " + this->message;
	}
	StringBuffer AudioException::ToString(StringType stringType) const
	{
		if (stringType == StringType::Normal)
		{
			return this->operator char *();
		}
		return this->operator wchar_t* ();
	}
	StringBuffer AudioException::ErrorCodeToHex() const
	{
		std::stringstream ss;
		ss << "0x" << std::setfill('0') << std::setw(sizeof(int64_t)) << std::hex << this->errorCode;
		return ss.str().c_str();
	}
}