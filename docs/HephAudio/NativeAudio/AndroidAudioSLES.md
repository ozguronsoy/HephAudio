## AndroidAudioSLES class
```c++
#include <NativeAudio/AndroidAudioSLES.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *[AndroidAudioBase](/docs/HephAudio/NativeAudio/AndroidAudioBase.md)* -> *AndroidAudioSLES*

### Description
Implements rendering/capturing audio data via [OpenSL ES](https://developer.android.com/ndk/guides/audio/opensl).

> [!NOTE]
> Requires Android 5.0 (API level 21) or higher.<br>

<br><br>

### Macros

```c++
#define HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL
```
The minimum API level required.
<br><br>

### Methods
```c++
AndroidAudioSLES();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
~AndroidAudioSLES();
```
Releases the resources.
<br><br><br><br>
