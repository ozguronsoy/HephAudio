## EventResult struct
```c++
#include <EventResult.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Base class for returning data from event handlers.
<br><br>


### Fields

```c++
struct EventResult
{
	bool isHandled;
}
```

- **isHandled**
<br><br>
Indicates whether the event is handled successfully and stops from calling the other event handlers.
<br><br>



### Methods

```c++
EventResult();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
virtual ~EventResult() = default;
```
Releases the resources.
