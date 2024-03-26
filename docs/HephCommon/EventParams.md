## EventParams struct
```c++
#include <EventParams.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Stores the event parameters passed to the event handlers.
<br><br>


### Fields

```c++
struct EventParams final
{
    EventArgs* pArgs;
    EventResult* pResult;
    const UserEventArgs& userEventArgs;
}
```

- **pArgs**
<br><br>
Pointer to the [EventArgs](/docs/HephCommon/EventArgs.md) instance.
<br><br>

- **pResult**
<br><br>
Pointer to the [EventResult](/docs/HephCommon/EventResult.md) instance.
<br><br>

- **userEventArgs**
<br><br>
Extra arguments passed by the user.
<br><br>


### Methods

```c++
EventParams(const UserEventArgs& userEventArgs);
```
Creates an instance and initializes it with the provided values.
- **userEventArgs:** Extra arguments passed by the user.
