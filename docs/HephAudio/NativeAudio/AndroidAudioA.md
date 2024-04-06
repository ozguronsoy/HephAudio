## AndroidAudioA class
```c++
#include <NativeAudio/AndroidAudioA.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *[AndroidAudioBase](/docs/HephAudio/NativeAudio/AndroidAudioBase.md)* -> *AndroidAudioA*

### Description
Implements rendering/capturing audio data via [AAudio](https://developer.android.com/ndk/guides/audio/aaudio/aaudio).

> [!NOTE]
> Requires Android 8.0 (API level 26) or higher.<br>

<br><br>

### Macros

```c++
#define HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL
```
The minimum API level required.
<br><br>

### Methods
```c++
AndroidAudioA();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
~AndroidAudioA();
```
Releases the resources.
<br><br><br><br>
