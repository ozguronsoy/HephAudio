## IAudioFileFormat class
```c++
#include <AudioFormats/IAudioFileFormat.h>
using namespace HephAudio::FileFormats;
```

> [Description](#description)<br>
[Methods](#methods)<br>
[Protected Fields](#protected-fields)



### Description
Base class for reading/writing audio files.
<br><br>


### Methods

```c++
virtual ~IAudioFileFormat() = default;
```
Releases the resources.
<br><br><br><br>

```c++
virtual std::string Extensions();
```
Gets the file extensions that are used for this format.
- **returns:** String of extensions each seperated by whitespace.
<br><br><br><br>

```c++
virtual bool VerifyExtension(const std::string& extension);
```
Checks whether the provided extension is used for this format.
- **extension:** Extension that will be checked.
- **returns:**  ``true`` if the extension is used for this format, otherwise ``false``.
<br><br><br><br>

```c++
virtual bool VerifySignature(const HephCommon::File& audioFile);
```
Verifies the file format signature.
- **audioFile:** File that will be verified.
- **returns:** ``true`` if the file is verified, otherwise ``false``.
<br><br><br><br>

```c++
virtual size_t FileFrameCount(const HephCommon::File& audioFile,
                              const AudioFormatInfo& audioFormatInfo);
```
Gets the number of audio frames the file contains.
- **audioFile:** Audio file.
- **audioFormatInfo:** Format info obtained by calling the ``ReadAudioFormatInfo`` method.
- **returns:** Number of audio frames.
<br><br><br><br>

```c++
virtual AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File& audioFile);
```
Reads the format info.
- **audioFile:** Audio file.
- **returns:** Format info.
<br><br><br><br>

```c++
virtual AudioBuffer ReadFile(const HephCommon::File& audioFile);
```
Reads the whole file and decodes to internal format.
- **audioFile:** Audio file.
- **returns:** Audio data.
<br><br><br><br>

```c++
virtual AudioBuffer ReadFile(const HephCommon::File& audioFile,
                             Codecs::IAudioCodec* pAudioCodec,
                             const AudioFormatInfo& audioFormatInfo,
                             size_t frameIndex,
                             size_t frameCount,
                             bool* finishedPlaying);
```
Reads and decodes the specified portion of the file.
- **audioFile:** Audio file.
- **pAudioCodec:** Codec the audio data is encoded with.
- **audioFormatInfo:** Format info obtained by calling the ``ReadAudioFormatInfo`` method.
- **frameIndex:** Index of the first frame that will be read.
- **frameCount:** Number of frames to read.
- **finishedPlaying:** Indicates whether the EOF is reached.
- **returns:** Audio data.
<br><br><br><br>


```c++
virtual bool SaveToFile(const std::string& filePath,
                        AudioBuffer& buffer,
                        bool overwrite);
```
Saves the audio data to a file.
- **filePath:** Path of the file.
- **buffer:** Audio data that will be saved.
- **overwrite:** Indicates whether to write over the file if it already exists.
- **returns:** ``true`` if the save operation succeeded, otherwise ``false``.
<br><br><br><br>


### Protected Fields

```c++
class IAudioFileFormat
{
    FFmpegAudioDecoder ffmpegAudioDecoder;
}
```

- ``ffmpegAudioDecoder``
<br><br>
FFmpeg decoder.

> [!NOTE]
> This field only exists when FFmpeg is enabled.

<br><br>
