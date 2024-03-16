## HephAudioShared
```c++
#include <HephAudioShared.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Data Types](#data-types)<br>
[Methods](#methods)

### Description
Defines macros, data types, and methods that will be used in all HephAudio header files.
<br><br>

### Macros
```c++
#define HEPHAUDIO_FORMAT_TAG_PCM
#define HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#define HEPHAUDIO_FORMAT_TAG_ALAW
#define HEPHAUDIO_FORMAT_TAG_MULAW
#define HEPHAUDIO_FORMAT_TAG_QUALCOMM_PUREVOICE
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO2
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO3
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO_LOSSLESS
#define HEPHAUDIO_FORMAT_TAG_WMAVOICE9
#define HEPHAUDIO_FORMAT_TAG_WMAVOICE10
#define HEPHAUDIO_FORMAT_TAG_MPEG
#define HEPHAUDIO_FORMAT_TAG_MPEGLAYER3
#define HEPHAUDIO_FORMAT_TAG_ALAC
#define HEPHAUDIO_FORMAT_TAG_OPUS
#define HEPHAUDIO_FORMAT_TAG_VORBIS
#define HEPHAUDIO_FORMAT_TAG_MPEG4_AAC
#define HEPHAUDIO_FORMAT_TAG_FLAC
#define HEPHAUDIO_FORMAT_TAG_EXTENSIBLE
```
<br>

```c++
// these macros are for internal use
// and will only be active if ``HEPHAUDIO_INFO_LOGGING`` is present in the compiler options
  
#define HEPHAUDIO_STOPWATCH_START
#define HEPHAUDIO_STOPWATCH_RESET
#define HEPHAUDIO_STOPWATCH_DT(prefix)
#define HEPHAUDIO_STOPWATCH_STOP
#define HEPHAUDIO_LOG(logMessage, logType)
```
<br>

```c++
#define HEPHAUDIO_SPEED_OF_SOUND 343.0
```
Speed of sound in dry air at 20°C in meters per second.
<br><br>

```c++
#define HEPH_AUDIO_SAMPLE_MIN -1
#define HEPH_AUDIO_SAMPLE_MAX 1
#define HEPH_AUDIO_SAMPLE heph_audio_sample // explained in the data types section
```
  <br><br>
### Data Types
```c++
typedef float heph_audio_sample;
```
Type of the audio samples, float by default. Can be changed by adding one of the following macros to the compiler options:
- ``HEPHAUDIO_SAMPLE_TYPE_FLOAT64`` for double
- ``HEPHAUDIO_SAMPLE_TYPE_FLOAT32`` for float
- ``HEPHAUDIO_SAMPLE_TYPE_HEPH_FLOAT`` for heph_float, which is explained in the HephCommonFramework docs
<br><br>

### Methods
```c++
inline heph_float DecibelToGain(heph_float decibel);
```
Converts decibel to amplitude.
- **decibel:** The value that will be converted.
- **Returns:** The amplitude.
<br><br><br><br>
  
```c++
inline heph_float GainToDecibel(heph_float gain);
```
Converts amplitude to decibel.
- **gain:** The value that will be converted.
- **Returns:** The gain in dB.
<br><br><br><br>
  
```c++
inline constexpr heph_float SemitoneToCent(heph_float semitone);
```
Converts semitone to cent.
- **semitone:** The value that will be converted.
- **Returns:** The converted value.
<br><br><br><br>

```c++
inline constexpr heph_float SemitoneToOctave(heph_float semitone);
```
Converts semitone to octave.
- **semitone:** The value that will be converted.
- **Returns:** The converted value.
<br><br><br><br>

```c++
inline constexpr heph_float CentToSemitone(heph_float cent);
```
Converts cent to semitone.
- **cent:** The value that will be converted.
- **Returns:** The converted value.
<br><br><br><br>

```c++
inline constexpr heph_float CentToOctave(heph_float cent);
```
Converts cent to octave.
- **cent:** The value that will be converted.
- **Returns:** The converted value.
<br><br><br><br>

```c++
inline constexpr heph_float OctaveToSemitone(heph_float octave);
```
Converts octave to semitone.
- **cent:** The value that will be converted.
- **Returns:** The converted value.
<br><br><br><br>

```c++
inline constexpr heph_float OctaveToCent(heph_float octave);
```
Converts octave to cent.
- **cent:** The value that will be converted.
- **Returns:** The converted value.
