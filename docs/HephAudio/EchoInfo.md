## EchoInfo Struct
```c++
#include <EchoInfo.h>
using namespace HephAudio;
```

### Description
Container for echo effect parameters.
<br><br>

### Parameters
```c++
struct EchoInfo
{
    uint32_t reflectionCount;
    heph_float reflectionDelay_s;
    heph_float decayFactor;
    heph_float echoStartPosition;
    heph_float echoEndPosition;
};
```

- ``reflectionCount``
<br><br>
Number of times the audio data will be reflected (repeated).
<br><br>

- ``reflectionDelay_s``
<br><br>
Elapsed time, in seconds, between the start of each reflection.
<br><br>

- ``decayFactor``
<br><br>
Multiplication factor of the reflected data. Reflected data will be multiplied by this between each reflection.
<br><br>

- ``echoStartPosition``
<br><br>
Start position of the audio data that will be reflected. Should be between 0 and 1.
<br><br>

- ``echoEndPosition``
<br><br>
End position of the audio data that will be reflected. Should be between 0 and 1.
<br><br>

> [!CAUTION]
> The ``echoStartPosition`` must be less than or equal to ``echoEndPosition``.

<br><br>

### Methods

```c++
EchoInfo();
```
Creates an instance and initializes it with default values.
<br><br><br><br>

```c++
EchoInfo(uint32_t reflectionCount,
         heph_float reflectionDelay_s,
         heph_float decayFactor,
         heph_float echoStartPosition,
         heph_float echoEndPosition);
```
Creates an instance and initializes it with the provided values.
<br><br><br><br>

```c++
size_t CalculateAudioBufferFrameCount(const AudioBuffer& buffer) const;
```
Calculates the number of frames the buffer will have after applying the echo effect.
- **buffer:** Buffer that the echo will be applied to.
- **Returns:** The number of frames buffer will have after applying the echo effect.
