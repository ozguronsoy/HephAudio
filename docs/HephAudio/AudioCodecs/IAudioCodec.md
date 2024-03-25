## IAudioCodec class
```c++
#include <AudioCodecs/IAudioCodec.h>
using namespace HephAudio::Codecs;
```

> [Description](#description)<br>
[Methods](#methods)



### Description
Base class for decoding/encoding audio data.
<br><br>


### Methods

```c++
virtual ~IAudioCodec() = default;
```
Releases the resources.
<br><br><br><br>

```c++
virtual uint32_t Tag();
```
Gets the codec tag.
- **returns:** Codec tag.
<br><br><br><br>

```c++
virtual AudioBuffer Decode(const EncodedBufferInfo& encodedBufferInfo);
```
Decodes the audio data to the internal format.
- **encodedBufferInfo:** Struct that contains the encoded data and other necessary information.
- **returns:** Decoded audio data.
<br><br><br><br>

```c++
virtual void Encode(AudioBuffer& bufferToEncode, EncodedBufferInfo& encodedBufferInfo);
```
Encodes the audio data.
- **bufferToEncode:** Audio data in internal format.
- **encodedBufferInfo:** Struct that contains the necessary information.
<br><br><br><br>
