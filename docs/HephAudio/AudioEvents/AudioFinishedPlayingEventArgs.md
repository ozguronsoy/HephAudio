## AudioFinishedPlayingEventArgs struct
```c++
#include <AudioEvents/AudioFinishedPlayingEventArgs.h>
using namespace HephAudio;
```
**Inheritance:** *[EventArgs](/docs/HephCommon/EventArgs.md)* -> *[AudioEventArgs](/docs/HephAudio/AudioEvents/AudioEventArgs.md)* -> *AudioFinishedPlayingEventArgs*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Stores the necessary arguments to handle an event.
<br><br>



### Fields
```c++
struct AudioFinishedPlayingEventArgs : public AudioEventArgs
{
    void* pAudioObject;
    uint32_t remainingLoopCount;
}
```

- ``pAudioObject``
<br><br>
Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance that's finished playing.
<br><br>

- ``remainingLoopCount``
<br><br>
Remaining number of times the [AudioObject](/docs/HephAudio/AudioObject.md) will be played.
<br><br>


### Methods

```c++
AudioFinishedPlayingEventArgs(void* pAudioObject,
                              void* pNativeAudio,
                              uint32_t remainingLoopCount);
```
Creates an instance and initializes it with the provided values.
- **pAudioObject:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance that's finished playing.
- **pNativeAudio:** Pointer to the [NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md) instance that raised the event.
- **remainingLoopCount:** Remaining number of times the [AudioObject](/docs/HephAudio/AudioObject.md) will be played.
