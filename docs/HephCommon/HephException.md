## HephException struct
```c++
#include <HephException.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Stores information about an exception.
<br><br>



### Macros
```c++
#define HEPH_EC_NONE
#define HEPH_EC_FAIL
#define HEPH_EC_INVALID_ARGUMENT
#define HEPH_EC_NOT_IMPLEMENTED
#define HEPH_EC_INSUFFICIENT_MEMORY
#define HEPH_EC_NOT_FOUND
#define HEPH_EC_INVALID_OPERATION
```
Error codes.
<br><br>

```c++
#define RAISE_HEPH_EXCEPTION(sender, ex)
```
Raises the ``HephException::OnException`` event.
- **sender:** Pointer to the instance that raised the exception. Can be ``nullptr``.
- **ex:** A ``HephException`` instance.
<br><br>

```c++
#define RAISE_AND_THROW_HEPH_EXCEPTION(sender, ex)
```
Raises the ``HephException::OnException`` event first, then throws.
- **sender:** Pointer to the instance that raised the exception. Can be ``nullptr``.
- **ex:** A ``HephException`` instance.
<br><br>


### Fields

```c++
struct HephException final
{
    static Event OnException;
    int64_t errorCode;
    std::string method;
    std::string message;
    std::string externalSource;
    std::string externalMessage;
}
```

- **OnException**
<br><br>
[Event](/docs/HephCommon/Event.md) that will be raised when an exception occurs.
<br><br>

- **errorCode**
<br><br>
The error code.
<br><br>

- **method**
<br><br>
The name of the method where the exception was raised.
<br><br>

- **message**
<br><br>
The error message.
<br><br>

- **externalSource**
<br><br>
Name of the external source (native API, some other library, etc.) which caused the exception.
<br><br>

- **externalMessage**
<br><br>
The error message returned by the external source.
<br><br>



### Methods

```c++
HephException();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
HephException(int64_t errorCode,
              std::string method,
              std::string message);
```
Creates an instance and initializes it with the provided values.
<br><br><br><br>

```c++
HephException(int64_t errorCode,
              std::string method,
              std::string message,
              std::string externalSource,
              std::string externalMessage);
```
Creates an instance and initializes it with the provided values.
<br><br><br><br>

```c++
void Raise(const void* pSender) const;
```
Raises the ``HephException::OnException`` event.
- **pSender:** Pointer to the instance that raised the exception. Can be ``nullptr``.
<br><br><br><br>

```c++
static const HephException& LastException();
```
Gets the last ``HephException`` that was raised by the current thread.
- **returns:** The last exception that was raised by the current thread if there is any, otherwise a default instance.
<br><br><br><br>

```c++
static const HephException& GetException(size_t index);
```
Gets the ``HephException`` that was raised by the current thread.
- **index:** Index of the desired exception. ``0`` to get the first exception.
- **returns:** The exception that was raised by the current thread, or a default instance if the index is out of bounds.
<br><br><br><br>

```c++
static size_t GetExceptionCount();
```
Gets the number of exceptions raised by the current thread.
- **returns:** Number of exceptions that were raised by the current thread.










