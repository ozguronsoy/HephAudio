## AudioEventArgs struct
```c++
#include <AudioEvents/AudioEventArgs.h>
using namespace HephAudio;
```
**Inheritance:** *[EventArgs](/docs/HephCommon/EventArgs.md)* -> *AudioEventArgs*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)


### Descriptions
Base struct for audio event args.

<br><br>


### Fields

```c++
struct AudioEventArgs : public HephCommon::EventArgs
{
    void* pNativeAudio;
}
```

- ``pNativeAudio``
<br><br>
Pointer to the [NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md) instance that raised the event.
<br><br>


### Methods

```c++
AudioEventArgs(void* pNativeAudio);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the [NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md) instance that raised the event.
<br><br><br><br>

```c++
virtual ~AudioEventArgs() = default;
```
Releases the resources.
