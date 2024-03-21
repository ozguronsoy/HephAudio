## FFmpegAudioShared
```c++
#include <FFmpeg/FFmpegAudioShared.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Methods](#methods)

<br><br>


> [!IMPORTANT]
> HephAudio provides neither the source code nor the binaries of FFmpeg. 
> Hence you have to obtain them yourself and also enable FFmpeg via a preprocessor directive.
> See [EnableFFmpeg](/docs/tutorials/EnableFFmpeg.md) for a step-by-step guide.

<br><br>

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
