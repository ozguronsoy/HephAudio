## AudioCodecManager class
```c++
#include <AudioCodecs/AudioCodecManager.h>
using namespace HephAudio::Codecs;
```

> [Description](#description)<br>
[Methods](#methods)


### Description
Class for managing the [IAudioCodec](/docs/HephAudio/AudioCodecs/IAudioCodec.md) instances.

> [!NOTE]
> This class only contains static methods and cannot be instantiated.

<br><br>


### Methods

```c++
static IAudioCodec* FindCodec(uint32_t codecTag);
```
Finds the codec with the provided tag.
- **codecTag:** Tag of the desired codec.
- **returns:** Pointer to the [IAudioCodec](/docs/HephAudio/AudioCodecs/IAudioCodec.md) instance if found, otherwise ``nullptr``.
<br><br><br><br>

```c++
static void RegisterCodec(IAudioCodec* pNewCodec);
```
Registers the provided codec implementation. If an implementation for that codec is already registered, removes it first.
- **pNewCodec:** Pointer to the new [IAudioCodec](/docs/HephAudio/AudioCodecs/IAudioCodec.md) instance.
