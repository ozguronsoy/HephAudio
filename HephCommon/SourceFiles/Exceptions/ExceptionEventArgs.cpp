#include "Exceptions/ExceptionEventArgs.h"

namespace Heph
{
	ExceptionEventArgs::ExceptionEventArgs(const void* pSender, const Exception& ex) : pSender(pSender), exception(ex) {}
}