## AudioDevice struct

### Description
Stores information about an audio device provided by the native API.

### Syntax
```c++
struct AudioDevice
{
    HephCommon::StringBuffer id;
    HephCommon::StringBuffer name;
    AudioDeviceType type;
    bool isDefault;
};
```

### Parameters
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
<br><br>
