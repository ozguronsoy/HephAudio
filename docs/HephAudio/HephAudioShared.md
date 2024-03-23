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
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO2
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO3
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO_LOSSLESS
#define HEPHAUDIO_FORMAT_TAG_WMAVOICE9
#define HEPHAUDIO_FORMAT_TAG_WMAVOICE10
#define HEPHAUDIO_FORMAT_TAG_MPEG
#define HEPHAUDIO_FORMAT_TAG_MP3
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
// and will only be active if ``HEPHAUDIO_INFO_LOGGING`` is defined
  
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
#define HEPH_AUDIO_SAMPLE_MIN
```
Minimum value the ``heph_audio_sample`` can have.

```c++
#define HEPH_AUDIO_SAMPLE_MAX
```
Maximum value the ``heph_audio_sample`` can have.

```c++
#define HEPH_AUDIO_SAMPLE heph_audio_sample // explained in the data types section
```

```c++
#define HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(sample)
```
``[HEPH_AUDIO_SAMPLE_MIN, HEPH_AUDIO_SAMPLE_MAX]`` -> ``[-1, 1]``

```c++
#define HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(fltSample)
```
``[-1, 1]`` -> ``[HEPH_AUDIO_SAMPLE_MIN, HEPH_AUDIO_SAMPLE_MAX]``


  <br><br>
### Data Types
```c++
typedef float heph_audio_sample;
```
Type of the audio samples, float by default. Can be changed by defining one of the following macros:
- ``HEPHAUDIO_SAMPLE_TYPE_DBL`` to select ``double``.
- ``HEPHAUDIO_SAMPLE_TYPE_FLT`` to select ``float``.
- ``HEPHAUDIO_SAMPLE_TYPE_S64`` to select ``int64_t``.
- ``HEPHAUDIO_SAMPLE_TYPE_S32`` to select ``int32_t``.
- ``HEPHAUDIO_SAMPLE_TYPE_S16`` to select ``int16_t``.

> [!IMPORTANT]
> HephAudio was initially designed to only use ``float`` or ``double`` as ``heph_audio_sample``.
> Hence some features (mostly sound effects) may not work properly for the integer types.


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
