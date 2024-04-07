## AudioDevice Struct
```c++
#include <AudioDevice.h>
using namespace HephAudio;
```

### Description
Stores information about an audio device provided by the native API.
<br><br>

### Enums
```c++
enum class AudioDeviceType : uint8_t
{
    Null = 0,
    Render = 1,
    Capture = 2,
    All = Render | Capture
};
```
<br><br>

### Fields
```c++
struct AudioDevice
{
    std::string id;
    std::string name;
    AudioDeviceType type;
    bool isDefault;
};
```

- ``id``
<br><br>
Unique identifier of the audio device.
<br><br>
- ``name``
<br><br>
Human-readable name of the audio device.
<br><br>
- ``type``
<br><br>
Indicates whether the device is an input (capture) or an output (render) device.
<br><br>
- ``isDefault``
<br><br>
Indicates whether the device is the system default.
