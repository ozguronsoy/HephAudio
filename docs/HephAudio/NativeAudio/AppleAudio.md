## AppleAudio class
```c++
#include <NativeAudio/AppleAudio.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *AppleAudio*

> [Description](#description)<br>
[Methods](#methods)

### Description
Implements rendering/capturing audio data and device enumeration via [CoreAudio](https://developer.apple.com/documentation/coreaudio).
<br><br>

### Methods
```c++
AppleAudio();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
~AppleAudio();
```
Releases the resources.
<br><br><br><br>
