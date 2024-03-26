## Event class
```c++
#include <Event.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Data Types](#data-types)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Manages the callback functions and the parameters passed to them.
<br><br>


### Data Types

```c++
typedef void (*EventHandler)(const EventParams& eventParams);
```
Defines the callback method that will be used for handling events.

<br><br>


### Fields

```c++
class Event final
{
    UserEventArgs userEventArgs;
}
```

- **userEventArgs**
<br><br>
Contains the extra arguments passed by the user.
<br><br>


### Methods

```c++
Event();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
Event(const EventHandler& handler);
```
Creates an instance and initializes it with the provided values.
- **handler:** The event handler.
<br><br><br><br>

```c++
operator bool() const;
```
Checks whether any handlers are assigned.
<br><br><br><br>

```c++
void operator()(EventArgs* pArgs, EventResult* pResult) const;
```
Calls the event handlers.
- **pArgs:** Pointer to an [EventArgs](/docs/HephCommon/EventArgs.md) instance.
- **pResult:** Pointer to an [EventResult](/docs/HephCommon/EventResult.md) instance.
<br><br><br><br>

```c++
Event& operator=(EventHandler handler);
```
Removes all event handlers, then adds the provided one.
- **handler:** The event handler.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
Event& operator+=(EventHandler handler);
```
Adds the provided event handler.
- **handler:** The event handler.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
Event& operator-=(EventHandler handler);
```
Removes the provided event handler.
- **handler:** The event handler.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
size_t EventHandlerCount() const;
```
Gets the number of event handlers.
- **returns:** Number of event handlers.
<br><br><br><br>

```c++
bool EventHandlerExists(EventHandler handler) const;
```
Checks whether the provided event handler is added.
- **handler:** The event handler.
- **returns:** ``true`` if the provided handler exists, otherwise ``false``.
<br><br><br><br>

```c++
EventHandler GetEventHandler(size_t index) const;
```
Gets the event handler at the provided index.
- **index:** Index of the desired event handler.
- **returns:** The event handler.
<br><br><br><br>

```c++
void SetEventHandler(EventHandler handler);
```
Same as ``operator=``.
<br><br><br><br>

```c++
void AddEventHandler(EventHandler handler);
```
Same as ``operator+=``.
<br><br><br><br>

```c++
void InsertEventHandler(EventHandler handler, size_t index);
```
Inserts the event handler to the provided index.
- **handler:** The event handler.
- **index:** Index.
<br><br><br><br>

```c++
void RemoveEventHandler(EventHandler handler);
```
Same as ``operator-=``.
<br><br><br><br>

```c++
void RemoveEventHandler(size_t index);
```
Removes the event handler at the provided index.
- **index:** Index of the event handler that will be removed.
<br><br><br><br>

```c++
void ClearEventHandlers();
```
Removes all of the event handlers.
<br><br><br><br>

```c++
void ClearAll();
```
Removes all of the event handlers and all of the user arguments.
<br><br><br><br>

```c++
void Invoke(EventArgs* pArgs, EventResult* pResult) const;
```
Same as ``operator()``.
