## FFmpegAudioEncoder
```c++
#include <FFmpeg/FFmpegAudioEncoder.h>
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

Class for creating audio files and encoding audio data via FFmpeg. 


<br><br>


### Methods

```c++
FFmpegAudioEncoder();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
FFmpegAudioEncoder(const HephCommon::StringBuffer& audioFilePath,
                   AudioFormatInfo outputFormatInfo,
                   bool overwrite);
```
Creates an instance and initializes it with the provided values.
- **audioFilePath:** Path of the file that will store the encoded data.
- **outputFormatInfo:** Desired properties of the encoded data.
- **overwrite:** Indicates whether to write over the file if it already exists.
<br><br><br><br>

```c++
FFmpegAudioEncoder(FFmpegAudioEncoder&& rhs) noexcept;
```
Creates an instance and moves the ownership of the contents of rhs to it.
- **rhs:** The instance whose contents will be moved.
<br><br><br><br>

```c++
~FFmpegAudioEncoder();
```
Releases the resources.
<br><br><br><br>

```c++
FFmpegAudioEncoder& operator=(FFmpegAudioEncoder&& rhs) noexcept;
```
Releases the resources of the current instance, then moves the ownership of the contents of ``rhs`` to it.
- **rhs:** The instance whose contents will be moved.
<br><br><br><br>

```c++
void ChangeFile(const HephCommon::StringBuffer& newAudioFilePath,
                bool overwrite);
```
Closes the current file and opens the new file.
- **newAudioFilePath:** Path of the file that will store the encoded data.
- **overwrite:** Indicates whether to write over the file if it already exists.
<br><br><br><br>

```c++
void CloseFile();
```
Releases the resources allocated for encoding and closes the file.
<br><br><br><br>

```c++
bool IsFileOpen() const;
```
Checks whether a file is currently open for encoding.
- **returns:** ``true`` if a file has successfully opened and allocated resources for encoding, otherwise ``false``.
<br><br><br><br>

```c++
void Encode(const AudioBuffer& bufferToEncode);
```
Encodes the provided buffer and appends it to the file.
- **bufferToEncode:** Audio data that will be encoded.
<br><br><br><br>

```c++
static void Transcode(const HephCommon::StringBuffer& inputFilePath,
                      const HephCommon::StringBuffer& outputFilePath,
                      bool overwrite);
```
Converts the input file format to the output file format and saves it. 
Uses the file extensions to determine the output codec.
- **inputFilePath:** Path of the input file.
- **outputFilePath:** Path of the output file.
- **overwrite:** Indicates whether to write over the output file if it already exists.
<br><br><br><br>

```c++
static void Transcode(const HephCommon::StringBuffer& inputFilePath,
                      const HephCommon::StringBuffer& outputFilePath,
                      AudioFormatInfo outputFormatInfo,
                      bool overwrite);
```
Converts the input file format to the output file format and saves it. 
Uses the file extensions to determine the output codec.
- **inputFilePath:** Path of the input file.
- **outputFilePath:** Path of the output file.
- **outputFormatInfo:** Desired properties of the encoded data.
- **overwrite:** Indicates whether to write over the output file if it already exists.
<br><br><br><br>
