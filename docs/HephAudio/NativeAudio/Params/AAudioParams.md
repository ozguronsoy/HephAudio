## AAudioParams struct
```c++
#include <NativeAudio/Params/AAudioParams.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudioParams](/docs/HephAudio/NativeAudio/Params/NativeAudioParams.md)* -> *AAudioParams*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Struct for storing the AAudio specific parameters.
<br><br>


### Fields

```c++
struct AAudioParams final : public NativeAudioParams
{
    aaudio_sharing_mode_t renderShareMode;
    aaudio_sharing_mode_t captureShareMode;
    aaudio_performance_mode_t renderPerformanceMode;
    aaudio_performance_mode_t capturePerformanceMode;
    heph_float renderBufferDuration_ms;
    heph_float captureBufferDuration_ms;
    aaudio_content_type_t renderContentType;
    aaudio_content_type_t captureContentType;
    aaudio_usage_t renderUsage;
    aaudio_usage_t captureUsage;
};
```

- ``renderShareMode``
<br><br>
Render share mode, ``AAUDIO_SHARING_MODE_SHARED`` or ``AAUDIO_SHARING_MODE_EXCLUSIVE``.
<br><br>

- ``captureShareMode``
<br><br>
Capture share mode, ``AAUDIO_SHARING_MODE_SHARED`` or ``AAUDIO_SHARING_MODE_EXCLUSIVE``.
<br><br>

- ``renderPerformanceMode``
<br><br>
Render performance mode.
<br><br>

- ``capturePerformanceMode``
<br><br>
Capture performance mode.
<br><br>

- ``renderBufferDuration_ms``
<br><br>
Duration of the render buffer in milliseconds.
<br><br>

- ``captureBufferDuration_ms``
<br><br>
Duration of the capture buffer in milliseconds.
<br><br>

- ``renderContentType``
<br><br>
Render content type, Available since API level 28.
<br><br>

- ``captureContentType``
<br><br>
Capture content type, Available since API level 28.
<br><br>

- ``renderUsage``
<br><br>
Render usage type, Available since API level 28.
<br><br>

- ``captureUsage``
<br><br>
Capture usage type, Available since API level 28.
<br><br>

### Methods

```c++
AAudioParams();
```
Creates an instance and initializes it with default values.
