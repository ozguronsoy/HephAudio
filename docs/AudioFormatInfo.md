## AudioFormatInfo struct

[description](#description)<br>
[defines](#defines)<br>
[parameters](#parameters)<br>
[methods](#methods)
<br><br>

### Description
Stores information on how audio signals are represented digitally.
<br><br>

### Defines
- ```c++
  #define HEPHAUDIO_INTERNAL_FORMAT(channelCount, sampleRate)
  ```
Creates an ``AudioFormatInfo`` instance in the format HephAudio uses internally with the provided channel count and sample rate.
<br><br>

### Parameters
```c++
struct AudioFormatInfo
{
    uint16_t formatTag;
    uint16_t channelCount;
    uint32_t sampleRate;
    uint16_t bitsPerSample;
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
- ``sampleRate``
<br><br>
Number of samples required to represent a one-second-long audio signal.
<br><br>
- ``bitsPerSample``
<br><br>
Bit-depth, the number of bits required to represent a sample.
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
AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate);
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
- <b>rhs:</b> The format to compare to.
- <b>Returns:</b> true if two formats are the same, otherwise false.
<br><br><br><br>
```c++
bool operator!=(const AudioFormatInfo& rhs) const;
```
Checks if the current format is different from the one provided.
- <b>rhs:</b> The format to compare to.
- <b>Returns:</b> true if two formats are different, otherwise false.
<br><br><br><br>
```c++
uint16_t FrameSize() const;
```
Calculates the size, in bytes, an audio frame occupies in memory.
- <b>Returns:</b> The size of an audio frame in bytes.
<br><br><br><br>
```c++
uint32_t BitRate() const;
```
Calculates the number of bits required to represent a one-second-long audio signal.
- <b>Returns:</b> The number of bits required to represent a one-second-long audio signal.
<br><br><br><br>
```c++
uint32_t ByteRate() const;
```
Calculates the number of bytes required to represent a one-second-long audio signal.
- <b>Returns:</b> The number of bytes required to represent a one-second-long audio signal.
