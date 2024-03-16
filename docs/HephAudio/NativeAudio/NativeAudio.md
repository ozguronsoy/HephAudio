## NativeAudio class
```c++
#include <NativeAudio/NativeAudio.h>
using namespace HephAudio::Native;
```

> [Description](#description)<br>
[Parameters](#parameters)<br>
[Methods](#methods)

### Description
Base class for the classes that interact with the native audio APIs.
<br><br>

### Parameters
```c++
class NativeAudio
{
	HephCommon::Event OnAudioDeviceAdded;
	HephCommon::Event OnAudioDeviceRemoved;
	HephCommon::Event OnCapture;
}
```

- ``OnAudioDeviceAdded``
<br><br>
The event that will be invoked every time an audio device is enabled, connected, or became available for any other reason.
<br><br>

- ``OnAudioDeviceRemoved``
<br><br>
The event that will be invoked every time an audio device is disabled, disconnected, or became unavailable for any other reason.
<br><br>

- ``OnCapture``
<br><br>
The event that will be invoked every time new audio data is captured.
<br><br>

### Methods
See the [Audio](/docs/HephAudio/Audio.md) class for the method descriptions.
