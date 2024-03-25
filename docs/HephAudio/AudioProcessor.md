## AudioProcessor
```c++
#include <AudioProcessor.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Methods](#methods)



### Description
Defines sound effects, filters, and some other audio processing methods.

> [!NOTE]
> This class only contains static methods and cannot be instantiated.


> [!IMPORTANT]
> Methods in this class are designed for buffers that store audio data in the ***internal format***.


<br><br>

### Methods

```c++
static void ChangeBitsPerSample(AudioBuffer& buffer,
                                uint16_t outputBitsPerSample);
```
Changes the bit depth of the pcm audio data.
- **buffer:** Audio data that will be changed.
- **outputBitsPerSample:** New bit depth.
<br><br><br><br>

```c++
static void ChangeNumberOfChannels(AudioBuffer& buffer,
                                   uint16_t outputChannelCount);
```
Changes the number of channels (samples) an audio frame contains. 
This method only takes the average of all current channels, and copies it to the output channels. 
Do not use this method for proper upmixing or downmixing.
- **buffer:** Audio data that will be changed.
- **outputChannelCount:** New number of channels.
<br><br><br><br>

```c++
static void ChangeSampleRate(AudioBuffer& buffer,
                             uint32_t outputSampleRate);
```
Changes the sample rate.
- **buffer:** Audio data that will be changed.
- **outputSampleRate:** New sample rate.
<br><br><br><br>

```c++
static std::vector<HephCommon::FloatBuffer> SplitChannels(const AudioBuffer& buffer);
```
Gets the audio channels in separate buffers.
- **buffer:** Audio data.
- **returns:** A vector of buffers that each contains a channel's data.
<br><br><br><br>

```c++
static AudioBuffer MergeChannels(const std::vector<HephCommon::FloatBuffer>& channels,
                                 uint32_t sampleRate);
```
Merges the channels.
- **channels:** A vector of buffers that each contains a channel's data.
- **sampleRate:** Sample rate of the audio data.
- **returns:** Audio data.
<br><br><br><br>

```c++
static void ConvertToInnerFormat(AudioBuffer& buffer);
```
Converts the provided audio data to internal format.
- **buffer:** Audio data that will be converted.
<br><br><br><br>

```c++
static void ConvertToTargetFormat(AudioBuffer& buffer,
                                  AudioFormatInfo targetFormat);
```
Converts the provided audio data to the desired format.
- **buffer:** Audio data that will be converted.
- **targetFormat:** Desired format.
<br><br><br><br>

```c++
static void ChangeEndian(AudioBuffer& buffer);
```
Changes the endianness of the audio data. Example: ``s32le`` -> ``s32be``.
- **buffer:** Audio data that will be changed.
<br><br><br><br>

```c++
static void Reverse(AudioBuffer& buffer);
```
Reverses the audio data.
- **buffer:** Audio data that the effect will be applied to.
<br><br><br><br>

```c++
static void Echo(AudioBuffer& buffer,
                 EchoInfo info);
```
Adds echo to the audio data.
- **buffer:** Audio data that the effect will be applied to.
- **info:** An [EchoInfo](/docs/HephAudio/EchoInfo.md) instance that describes the echo characteristics.
<br><br><br><br>

```c++
static void LinearPanning(AudioBuffer& buffer,
                          heph_float panningFactor);
```
Linearly increases the volume of one channel while decreasing the other one (stereo only).
- **buffer:** Audio data that the effect will be applied to.
- **panningFactor:** Determines how the channels' volume will be changed. Ranges between -1 and 1.<br> -1 means only the left channel will be audible and 1 means only the right channel will be audible.
<br><br><br><br>

```c++
static void SquareLawPanning(AudioBuffer& buffer,
                          heph_float panningFactor);
```
Increases the volume of one channel while decreasing the other one (stereo only). Uses square root to calculate the volumes.
- **buffer:** Audio data that the effect will be applied to.
- **panningFactor:** Determines how the channels' volume will be changed. Ranges between -1 and 1.<br> -1 means only the left channel will be audible and 1 means only the right channel will be audible.
<br><br><br><br>

```c++
static void SineLawPanning(AudioBuffer& buffer,
                          heph_float panningFactor);
```
Increases the volume of one channel while decreasing the other one (stereo only). Uses sin function to calculate the volumes.
- **buffer:** Audio data that the effect will be applied to.
- **panningFactor:** Determines how the channels' volume will be changed. Ranges between -1 and 1.<br> -1 means only the left channel will be audible and 1 means only the right channel will be audible.
<br><br><br><br>

```c++
static void Tremolo(AudioBuffer& buffer,
                    heph_float depth,
                    const Oscillator& lfo);
```
Changes the volume of the provided audio data periodically.
- **buffer:** Audio data that the effect will be applied to.
- **depth:** Determines the dry/wet mix ratio.
- **lfo:** A low-frequency [oscillator](/docs/HephAudio/Oscillators/Oscillator.md).
<br><br><br><br>
