#include "HephException.h"

namespace HephCommon
{
	HephException::HephException() : HephException(0, "", "") { }
	HephException::HephException(int32_t errorCode, StringBuffer method, StringBuffer message)
		: errorCode(errorCode), method(method), message(message) {}
	HephException::operator char* () const
	{
		this->resultMessage = this->ToString(StringType::ASCII);
		return this->resultMessage;
	}
	HephException::operator wchar_t* () const
	{
		this->resultMessage = this->ToString(StringType::Wide);
		return this->resultMessage;
	}
	StringBuffer HephException::ToString(StringType stringType) const
	{
		if (stringType == StringType::ASCII)
		{
			return "Audio Exception " + StringBuffer::ToHexString(errorCode) + " (" + StringBuffer::ToString(this->errorCode) + ")\nMethod: " + this->method + "\nMessage: " + this->message;
		}

		StringBuffer hexCode = StringBuffer::ToHexString(this->errorCode);
		hexCode.SetStringType(StringType::Wide);
		return L"Audio Exception " + hexCode + L" (" + StringBuffer::ToString(this->errorCode) + L")\nMethod: " + this->method + L"\nMessage: " + this->message;
	}
}