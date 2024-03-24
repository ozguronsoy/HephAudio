## AudioCaptureEventArgs struct
```c++
#include <AudioEvents/AudioCaptureEventArgs.h>
using namespace HephAudio;
```
**Inheritance:** *[EventArgs](/docs/HephCommon/EventArgs.md)* -> *[AudioEventArgs](/docs/HephAudio/AudioEvents/AudioEventArgs.md)* -> *AudioCaptureEventArgs*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Stores the necessary arguments to handle an event.
<br><br>



### Fields
```c++
struct AudioCaptureEventArgs : public AudioEventArgs
{
    AudioBuffer captureBuffer;
}
```

- ``captureBuffer``
<br><br>
Captured audio data.
<br><br>


### Methods

```c++
AudioCaptureEventArgs(void* pNativeAudio,
                      AudioBuffer& captureBuffer);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the [NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md) instance that raised the event.
- **captureBuffer:** Captured audio data.
