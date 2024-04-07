## AudioFileFormatManager class
```c++
#include <AudioFormats/AudioFileFormatManager.h>
using namespace HephAudio::FileFormats;
```

> [Description](#description)<br>
[Methods](#methods)<br>



### Description
Class for managing the [IAudioFileFormat](/docs/HephAudio/AudioFormats/IAudioFileFormat.md) instances.
<br><br>




### Methods

```c++
static void RegisterFileFormat(IAudioFileFormat* pNewCodec);
```
Registers the provided file format implementation. If an implementation for that file format is already registered, removes it first.
- **pNewCodec:** Pointer to the new [IAudioFileFormat](/docs/HephAudio/AudioFormats/IAudioFileFormat.md) instance. This instance must live until the end of the program.
<br><br><br><br>

```c++
static IAudioFileFormat* FindFileFormat(const HephCommon::File& file);
```
Finds the format of the provided file.
- **file:** File.
- **returns:** Pointer to the [IAudioFileFormat](/docs/HephAudio/AudioFormats/IAudioFileFormat.md) instance if implemented, otherwise ``nullptr``.
<br><br><br><br>

```c++
static IAudioFileFormat* FindFileFormat(const std::string& filePath);
```
Finds the format of the file from its extension.
- **filePath:** File path.
- **returns:** Pointer to the [IAudioFileFormat](/docs/HephAudio/AudioFormats/IAudioFileFormat.md) instance if implemented, otherwise ``nullptr``.
<br><br><br><br>
