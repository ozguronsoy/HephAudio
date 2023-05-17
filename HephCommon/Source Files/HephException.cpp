#include "HephException.h"

namespace HephCommon
{
	Event HephException::OnException = Event();
	HephException::HephException() : errorCode(HephException::ec_none), method(""), message("") { }
	HephException::HephException(int64_t errorCode, StringBuffer method, StringBuffer message) : errorCode(errorCode), method(method), message(message) {}
}