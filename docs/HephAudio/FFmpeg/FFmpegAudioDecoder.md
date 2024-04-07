## FFmpegAudioDecoder
```c++
#include <FFmpeg/FFmpegAudioDecoder.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Methods](#methods)

<br><br>


> [!IMPORTANT]
> HephAudio provides neither the source code nor the binaries of FFmpeg. 
> Hence you have to obtain them yourself and also enable FFmpeg via a preprocessor directive.
> See [EnableFFmpeg](/docs/tutorials/EnableFFmpeg.md) for a step-by-step guide.

<br><br>

### Description

Class for reading and decoding audio files via FFmpeg.

<br><br>


### Methods

```c++
FFmpegAudioDecoder();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
FFmpegAudioDecoder(const std::string& audioFilePath);
```
Creates an instance and initializes it with the provided values.
- **audioFilePath:** Path of the file that will be decoded.
<br><br><br><br>

```c++
FFmpegAudioDecoder(FFmpegAudioDecoder&& rhs) noexcept;
```
Creates an instance and moves the ownership of the contents of rhs to it.
- **rhs:** The instance whose contents will be moved.
<br><br><br><br>

```c++
~FFmpegAudioDecoder();
```
Releases the resources.
<br><br><br><br>

```c++
FFmpegAudioDecoder& operator=(FFmpegAudioDecoder&& rhs) noexcept;
```
Releases the resources of the current instance, then moves the ownership of the contents of ``rhs`` to it.
- **rhs:** The instance whose contents will be moved.
<br><br><br><br>

```c++
void ChangeFile(const std::string& newAudioFilePath);
```
Changes the file that's being decoded.
- **newAudioFilePath:** Path of the file that will be decoded.
<br><br><br><br>

```c++
void CloseFile();
```
Releases the resources allocated for decoding the current file.
<br><br><br><br>

```c++
bool IsFileOpen() const;
```
Checks whether a file is currently open for decoding.
- **returns:** ``true`` if a file has successfully opened and allocated resources for decoding it, otherwise ``false``.
<br><br><br><br>

```c++
AudioFormatInfo GetOutputFormatInfo() const;
```
Gets the [AudioFormatInfo](/docs/HephAudio/AudioFormatInfo.md) of the decoded audio data.
- **returns:** Format info that describes the properties of the decoded audio data.
<br><br><br><br>

```c++
size_t GetFrameCount() const;
```
Gets the number of audio frames the file contains.
- **returns:** Number of audio frames the file contains.
<br><br><br><br>

```c++
bool Seek(size_t frameIndex);
```
Gets to the frame at the provided index.
- **returns:** ``true`` if the operation succeeded, otherwise ``false``.
<br><br><br><br>

```c++
AudioBuffer Decode();
```
Decodes the remaining part of the file.
- **returns:** Decoded data.
<br><br><br><br>

```c++
AudioBuffer Decode(size_t frameIndex, size_t frameCount);
```
Decodes the specified part of the file.
- **frameIndex:** Index of the first frame that will be decoded.
- **frameCount:** Number of frames to decode.
- **returns:** Decoded data.
<br><br><br><br>

```c++
AudioBuffer DecodeWholePackets(size_t minFrameCount);
```
Decodes the whole [AVPackets](https://ffmpeg.org/doxygen/3.2/structAVPacket.html). 
This method is used in [AudioStream](/docs/HephAudio/AudioStream.md) class.
- **minFrameCount:** Minimum number of frames to decode.
- **returns:** Decoded data.
<br><br><br><br>
