## AudioDeviceEventArgs struct
```c++
#include <AudioEvents/AudioDeviceEventArgs.h>
using namespace HephAudio;
```
**Inheritance:** *[EventArgs](/docs/HephCommon/EventArgs.md)* -> *[AudioEventArgs](/docs/HephAudio/AudioEvents/AudioEventArgs.md)* -> *AudioDeviceEventArgs*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Stores the necessary arguments to handle an event.
<br><br>



### Fields
```c++
struct AudioDeviceEventArgs : public AudioEventArgs
{
    AudioDevice audioDevice;
}
```

- ``audioDevice``
<br><br>
Audio device that caused the event.
<br><br>


### Methods

```c++
AudioDeviceEventArgs(void* pNativeAudio,
                     AudioDevice audioDevice);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the [NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md) instance that raised the event.
- **audioDevice:** Audio device that caused the event.
