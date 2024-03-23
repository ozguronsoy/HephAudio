## AudioFormatInfo Struct
```c++
#include <AudioFormatInfo.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Fields](#fields)<br>
[Methods](#methods)

### Description
Stores information on how audio signals are represented digitally.
<br><br>

### Macros
```c++
#define HEPHAUDIO_INTERNAL_FORMAT(channelCount, sampleRate)
```
Creates an ``AudioFormatInfo`` instance in the format HephAudio uses internally with the provided channel count and sample rate.
<br><br>

### Fields
```c++
struct AudioFormatInfo
{
    uint16_t formatTag;
    uint16_t channelCount;
    uint16_t bitsPerSample;
    uint32_t sampleRate;
    uint32_t bitRate;
    HephCommon::Endian endian;
};
```

- ``formatTag``
<br><br>
Tag of the format that audio data is encoded with.
<br><br>
- ``channelCount``
<br><br>
Number of channels present in the audio buffer. This will be 1 for Mono and 2 for Stereo audio. For stereo, the first channel will be played through the left, and the second will be played through the right.
<br><br>
- ``bitsPerSample``
<br><br>
Bit depth, the number of bits required to represent a sample.
<br><br>
- ``sampleRate``
<br><br>
Samples per second (Hz), the number of samples required to represent a one-second-long audio signal.
<br><br>
- ``bitRate``
<br><br>
Bits per second (bps), the number of bits required to represent a one-second-long audio signal.
<br><br>
- ``endian``
<br><br>
Endianness, the order in which bytes within a sample are stored.
<br><br>

### Methods

```c++
AudioFormatInfo();
```
Creates an instance and initializes it with default values.
<br><br><br><br>

```c++
AudioFormatInfo(uint16_t formatTag,
                uint16_t nChannels,
                uint16_t bps,
                uint32_t sampleRate);
```
Creates an instance and initializes it with provided values. Assigns ``HEPH_SYSTEM_ENDIAN`` to the ``AudioFormatInfo::endian`` field.
<br><br><br><br>

```c++
AudioFormatInfo(uint16_t formatTag,
                uint16_t nChannels,
                uint16_t bps,
                uint32_t sampleRate,
                HephCommon::Endian endian);
```
Creates an instance and initializes it with provided values.
<br><br><br><br>

```c++
bool operator==(const AudioFormatInfo& rhs) const;
```
Checks if the current format is the same as the one provided.
- **rhs:** The format to compare to.
- **returns:** true if two formats are the same, otherwise false.
<br><br><br><br>

```c++
bool operator!=(const AudioFormatInfo& rhs) const;
```
Checks if the current format is different from the one provided.
- **rhs:** The format to compare to.
- **returns:** true if two formats are different, otherwise false.
<br><br><br><br>

```c++
uint16_t FrameSize() const;
```
Calculates the size, in bytes, an audio frame occupies in memory.
- **returns:** The size of an audio frame in bytes.
<br><br><br><br>

```c++
uint32_t ByteRate() const;
```
Calculates the number of bytes required to represent a one-second-long audio signal.
- **returns:** The number of bytes required to represent a one-second-long audio signal.

```c++
static uint32_t CalculateBitrate(const AudioFormatInfo& formatInfo);
```
Calculates the bitrate of the ``PCM``, ``IEEE_FLOAT``, ``ALAW``, and ``MULAW`` codecs.
- **formatInfo:** Format info that will be used to calculate the bitrate.
- **returns:** Bitrate.
<br><br><br><br>
