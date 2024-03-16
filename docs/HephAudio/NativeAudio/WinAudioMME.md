## WinAudioMME class
```c++
#include <NativeAudio/WinAudioMME.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *[WinAudioBase](/docs/HephAudio/NativeAudio/WinAudioBase.md)* -> *WinAudioMME*

### Description
Implements rendering/capturing audio data and device enumeration via [MME API](https://learn.microsoft.com/en-us/windows/win32/api/mmeapi/).

> [!NOTE]
> MME API is a legacy API that's superseded by DirectSound and WASAPI and should be avoided whenever possible.

<br><br>

### Methods
```c++
WinAudioMME();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
~WinAudioMME();
```
Releases the resources.
<br><br><br><br>
