# Handling Exceptions

Every error that occurs while using the library raises an ``OnException`` event but only some exception will actually throw. Using this event we can log the exception details to a file or for this instance, simply print it to console.

```cpp
#include <Exceptions/Exception.h>
#include <Exceptions/InvalidArgumentException.h>
#include <Exceptions/InsufficientMemoryException.h>

using namespace Heph;

int main()
{
	Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

	HEPH_RAISE_EXCEPTION(nullptr, InvalidArgumentException(HEPH_FUNC, "invalid argument!"));
	
	HEPH_RAISE_AND_THROW_EXCEPTION(nullptr, InsufficientMemoryException(HEPH_FUNC, "insufficient memory!"));
	
	return 0;
}
```