## HephExceptionEventArgs struct
```c++
#include <HephExceptionEventArgs.h>
using namespace HephCommon;
```
**Inheritance:** *[EventArgs](/docs/HephCommon/EventArgs.md)* -> *HephExceptionEventArgs*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)


### Descriptions
Stores the necessary arguments to handle an exception event.
<br><br>



### Fields

```c++
struct HephExceptionEventArgs : public EventArgs
{
    const void* pSender;
    HephException exception;
}
```

- ``pSender``
<br><br>
Pointer to the instance that raised the exception. Can be ``nullptr``.
<br><br>

- ``exception``
<br><br>
The [HephException](/docs/HephCommon/HephException.md) instance.
<br><br>


### Methods

```c++
HephExceptionEventArgs(const void* pSender,
                       HephException ex);
```
<br><br><br><br>

```c++
virtual ~HephExceptionEventArgs() = default;
```
Releases the resources.
