## WinAudioDS class
```c++
#include <NativeAudio/WinAudioDS.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *[WinAudioBase](/docs/HephAudio/NativeAudio/WinAudioBase.md)* -> *WinAudioDS*

### Description
Implements rendering/capturing audio data and device enumeration via [DirectSound](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ee416960(v=vs.85)).

> [!NOTE]
> Requires Windows XP or higher.<br>
> DirectSound is a legacy API that's superseded by WASAPI. which is used in the [WinAudio](/docs/HephAudio/NativeAudio/WinAudio.md) class.

<br><br>

### Methods
```c++
WinAudioDS();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
~WinAudioDS();
```
Releases the resources.
<br><br><br><br>
