#pragma once
#include "framework.h"
#include "StringBuffer.h"
#include <cinttypes>

namespace HephAudio
{
	struct AudioException final
	{
	private:
		int32_t errorCode;
		StringBuffer method; // Method name that exception has occurred.
		StringBuffer message;
		mutable StringBuffer resultMessage;
	public:
		AudioException();
		AudioException(int32_t errorCode, StringBuffer method, StringBuffer message);
		operator char* () const;
		operator wchar_t* () const;
		StringBuffer ToString(StringType stringType) const;
	};
}