## OpenSLParams struct
```c++
#include <NativeAudio/Params/OpenSLParams.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudioParams](/docs/HephAudio/NativeAudio/Params/NativeAudioParams.md)* -> *OpenSLParams*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Struct for storing the OpenSL-ES specific parameters.
<br><br>


### Fields

```c++
struct OpenSLParams final : public NativeAudioParams
{
    heph_float renderBufferDuration_ms;
    heph_float captureBufferDuration_ms;
};
```

- ``renderBufferDuration_ms``
<br><br>
Duration of the render buffer in milliseconds.
<br><br>

- ``captureBufferDuration_ms``
<br><br>
Duration of the capture buffer in milliseconds.
<br><br>

### Methods

```c++
OpenSLParams();
```
Creates an instance and initializes it with default values.
