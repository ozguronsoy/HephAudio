## FFmpegAudioShared
```c++
#include <FFmpeg/FFmpegAudioShared.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Methods](#methods)

<br><br>

### Description

Includes the FFmpeg headers and defines helper methods.

<br><br>

### Macros

```c++
#define HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(errorCode)
```
Gets the error error description.

```c++
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT
```
Defines the [AVSampleFormat](https://ffmpeg.org/doxygen/trunk/group__lavu__sampfmts.html) that's internally used.

<br><br>

### Methods

```c++
inline std::string FFmpegGetErrorMessage(int errorCode);
```
Gets the error error description.
- **errorCode:** Negative error code returned by an FFmpeg method.
- **returns:** Error description.
<br><br><br><br>

```c++
inline AVCodecID CodecIdFromAudioFormatInfo(const AudioFormatInfo& audioFormatInfo);
```
Gets the [AVCodecID](https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gaadca229ad2c20e060a14fec08a5cc7ce) from the provided audio format info.
- **audioFormatInfo:** Audio format info.
- **returns:** AVCodecID.
<br><br><br><br>

```c++
inline AVChannelLayout ToAVChannelLayout(const AudioChannelLayout& audioChannelLayout);
```
Converts the [AudioChannelLayout](/docs/HephAudio/AudioChannelLayout.md) struct to [AVChannelLayout](https://ffmpeg.org/doxygen/trunk/structAVChannelLayout.html) struct.
<br><br><br><br>

```c++
inline AudioChannelLayout FromAVChannelLayout(const AVChannelLayout& avChannelLayout);
```
Converts the [AVChannelLayout](https://ffmpeg.org/doxygen/trunk/structAVChannelLayout.html) struct to [AudioChannelLayout](/docs/HephAudio/AudioChannelLayout.md) struct.
