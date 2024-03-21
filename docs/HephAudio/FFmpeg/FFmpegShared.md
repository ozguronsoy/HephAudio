## FFmpegAudioShared
```c++
#include <FFmpeg/FFmpegAudioShared.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Methods](#methods)


### Description

Includes the FFmpeg headers and defines helper methods.



### Macros

```c++
#define HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(errorCode)
```
Gets the error error description.

```c++
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT
```
Defines the [AVSampleFormat] that's internally used.


### Methods

```c++
inline HephCommon::StringBuffer FFmpegGetErrorMessage(int errorCode);
```
Gets the error error description.
- **errorCode:** Negative error code returned by an FFmpeg method.
