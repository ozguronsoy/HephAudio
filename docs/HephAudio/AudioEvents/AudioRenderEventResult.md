## AudioRenderEventResult struct
```c++
#include <AudioEvents/AudioRenderEventResult.h>
using namespace HephAudio;
```
**Inheritance:** *[EventResult](/docs/HephCommon/EventResult.md)* -> *AudioRenderEventResult*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Stores the data that will be used after the event is handled.
<br><br>



### Fields
```c++
struct AudioRenderEventResult : public HephCommon::EventResult
{
    AudioBuffer renderBuffer;
    bool isFinishedPlaying;
}
```

- ``renderBuffer``
<br><br>
Audio data that will be rendered.
<br><br>

- ``isFinishedPlaying``
<br><br>
Indicates whether to finish playing the [AudioObject](/docs/HephAudio/AudioObject.md).<br>
``false`` by default.
<br><br>


### Methods

```c++
AudioRenderEventResult();
```
Creates an instance and initializes it with the default values.
