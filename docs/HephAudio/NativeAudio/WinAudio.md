## WinAudio class
```c++
#include <NativeAudio/WinAudio.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *[WinAudioBase](/docs/HephAudio/NativeAudio/WinAudioBase.md)* -> *WinAudio*

> [Description](#description)<br>
[Methods](#methods)<br>

### Description
Implements rendering/capturing audio data and device enumeration via [WASAPI](https://learn.microsoft.com/en-us/windows/win32/coreaudio/wasapi).

> [!NOTE]
> Available for Windows Vista and higher.

<br><br>

### Methods

```c++
WinAudio();
```
Creates an instance and initializes it with default values.
<br><br><br><br>

```c++
~WinAudio();
```
Releases the resources.
<br><br><br><br>

```c++
void SetDisplayName(HephCommon::StringBuffer displayName);
```
Sets the name that's displayed in the Volume mixer app.
- **displayName:** Name that will be displayed in the Volume mixer app.
<br><br><br><br>

```c++
void SetIconPath(HephCommon::StringBuffer iconPath);
```
Sets the path to the icon that's displayed in the Volume mixer app.
- **displayName:** Path to the icon that will be displayed in the Volume mixer app.
<br><br><br><br>
