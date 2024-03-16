## LinuxAudio class
```c++
#include <NativeAudio/LinuxAudio.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *LinuxAudio*

### Description
Implements rendering/capturing audio data and device enumeration via **Advanced Linux Sound Architecture (ALSA)**.
<br><br>

### Methods
```c++
LinuxAudio();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
~LinuxAudio();
```
Releases the resources.
<br><br><br><br>
