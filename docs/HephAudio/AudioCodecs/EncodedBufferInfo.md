## EncodedBufferInfo class
```c++
#include <AudioCodecs/EncodedBufferInfo.h>
using namespace HephAudio::Codecs;
```

> [Description](#description)<br>
[Fields](#fields)


### Description
Stores the data that are necessary for encoding/decoding audio data.
<br><br>


### Fields

```c++
struct EncodedBufferInfo final
{
    void* pBuffer;
    size_t size_byte;
    size_t size_frame;
    AudioFormatInfo formatInfo;
};
```

- ``pBuffer``
<br><br>
Pointer to the encoded data, used for decoding only.
<br><br>

- ``size_byte``
<br><br>
Size of the audio data in bytes.
<br><br>

- ``size_frame``
<br><br>
Size of the audio data in frames. Number of audio frames.
<br><br>

- ``formatInfo``
<br><br>
Format of the audio data.
<br><br>
