#include "AudioException.h"

namespace HephAudio
{
	AudioException::AudioException() : AudioException(0, "", "") { }
	AudioException::AudioException(int32_t errorCode, StringBuffer method, StringBuffer message)
		: errorCode(errorCode), method(method), message(message) {}
	AudioException::operator char* () const
	{
		this->resultMessage = this->ToString(StringType::Normal);
		return this->resultMessage;
	}
	AudioException::operator wchar_t* () const
	{
		this->resultMessage = this->ToString(StringType::Wide);
		return this->resultMessage;
	}
	StringBuffer AudioException::ToString(StringType stringType) const
	{
		if (stringType == StringType::Normal)
		{
			return "Audio Exception " + StringBuffer::ToHexString(errorCode) + " (" + StringBuffer::ToString(this->errorCode) + ")\nMethod: " + this->method + "\nMessage: " + this->message;
		}

		StringBuffer hexCode = StringBuffer::ToHexString(this->errorCode);
		hexCode.SetStringType(StringType::Wide);
		return L"Audio Exception " + hexCode + L" (" + StringBuffer::ToString(this->errorCode) + L")\nMethod: " + this->method + L"\nMessage: " + this->message;
	}
}