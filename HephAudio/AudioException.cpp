#include "AudioException.h"

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
		this->resultMessage = "Audio Exception " + StringBuffer::ToHexString(errorCode) + " (" + StringBuffer::ToString(this->errorCode) + ")\nMethod: " + this->method + "\nMessage: " + this->message;
		return this->resultMessage;
	}
	AudioException::operator wchar_t* () const
	{
		StringBuffer hexCode = StringBuffer::ToHexString(this->errorCode);
		hexCode.SetStringType(StringType::Wide);
		this->resultMessage = L"Audio Exception " + hexCode + L" (" + StringBuffer::ToString(this->errorCode) + L")\nMethod: " + this->method + L"\nMessage: " + this->message;
		return this->resultMessage;
	}
	StringBuffer AudioException::ToString(StringType stringType) const
	{
		if (stringType == StringType::Normal)
		{
			return this->operator char* ();
		}
		return this->operator wchar_t* ();
	}
}