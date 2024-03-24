## AudioRenderEventArgs struct
```c++
#include <AudioEvents/AudioRenderEventArgs.h>
using namespace HephAudio;
```
**Inheritance:** *[EventArgs](/docs/HephCommon/EventArgs.md)* -> *[AudioEventArgs](/docs/HephAudio/AudioEvents/AudioEventArgs.md)* -> *AudioRenderEventArgs*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Stores the necessary arguments to handle an event.
<br><br>



### Fields
```c++
struct AudioRenderEventArgs : public AudioEventArgs
{
    void* pAudioObject;
    size_t renderFrameCount;
}
```

- ``pAudioObject``
<br><br>
Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance that's finished playing.
<br><br>

- ``renderFrameCount``
<br><br>
Number of frames that will be rendered.
<br><br>


### Methods

```c++
AudioRenderEventArgs(void* pAudioObject,
                     void* pNativeAudio,
                     size_t renderFrameCount);
```
Creates an instance and initializes it with the provided values.
- **pAudioObject:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance that's finished playing.
- **pNativeAudio:** Pointer to the [NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md) instance that raised the event.
- **renderFrameCount:** Number of frames that will be rendered.
