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
- **panningFactor:** Determines how the channels' volume will be changed. Range: ``[-1, 1]``.<br> -1 means only the left channel will be audible and 1 means only the right channel will be audible.
<br><br><br><br>

```c++
static void Tremolo(AudioBuffer& buffer,
                    heph_float depth,
                    const Oscillator& lfo);
```
Changes the volume of the provided audio data periodically and adds the result (wet) to the input (dry) signal.
- **buffer:** Audio data that the effect will be applied to.
- **depth:** Controls the dry/wet mix ratio. Range: ``[0, 1]``.
- **lfo:** A low-frequency [oscillator](/docs/HephAudio/Oscillators/Oscillator.md).
<br><br><br><br>

```c++
static void Vibrato(AudioBuffer& buffer,
                    heph_float depth,
                    heph_float extent_semitone,
                    const Oscillator& lfo);
```
Changes the pitch of the provided audio data periodically and adds the result (wet) to the input (dry) signal.
- **buffer:** Audio data that the effect will be applied to.
- **depth:** Controls the dry/wet mix ratio. Range: ``[0, 1]``.
- **extent_semitone:** Maximum pitch change in terms of semitones.
- **lfo:** A low-frequency [oscillator](/docs/HephAudio/Oscillators/Oscillator.md).
<br><br><br><br>

```c++
static void Chorus(AudioBuffer& buffer,
                   heph_float depth,
                   heph_float feedbackGain,
                   heph_float baseDelay_ms,
                   heph_float delay_ms,
                   heph_float extent_semitone,
                   const Oscillator& lfo);
```
Delays the provided audio data and changes its pitch periodically. 
Then adds the result (wet) to the input (dry) signal.
- **buffer:** Audio data that the effect will be applied to.
- **depth:** Controls the dry/wet mix ratio. Range: ``[0, 1]``.
- **feedbackGain:** Gain of the feedback sample.
- **baseDelay_ms:** Constant delay, in milliseconds, that will be added to the wet signal.
- **delay_ms:** Maximum value of the variable delay, in milliseconds, that will be added to the wet signal.
The delay applied to wet samples changes periodically. The maximum delay that will be applied can be calculated as ``baseDelay_ms + delay_ms``.
- **extent_semitone:** Maximum pitch change in terms of semitones.
- **lfo:** A low-frequency [oscillator](/docs/HephAudio/Oscillators/Oscillator.md).
<br><br><br><br>

```c++
static void Flanger(AudioBuffer& buffer,
                    heph_float depth,
                    heph_float feedbackGain,
                    heph_float baseDelay_ms,
                    heph_float delay_ms,
                    const Oscillator& lfo);
```
Delays the provided audio data and adds the result (wet) to the input (dry) signal. The amount of delay applied changes periodically.
- **buffer:** Audio data that the effect will be applied to.
- **depth:** Controls the dry/wet mix ratio. Range: ``[0, 1]``.
- **feedbackGain:** Gain of the feedback sample.
- **baseDelay_ms:** Constant delay, in milliseconds, that will be added to the wet signal.
- **delay_ms:** Maximum value of the variable delay, in milliseconds, that will be added to the wet signal.
The delay applied to wet samples changes periodically. The maximum delay that will be applied can be calculated as ``baseDelay_ms + delay_ms``.
- **lfo:** A low-frequency [oscillator](/docs/HephAudio/Oscillators/Oscillator.md).
<br><br><br><br>

```c++
static void FixOverflow(AudioBuffer& buffer);
```
Fixes the distortion due to floating point samples exceeding the [-1, 1] range.
- **buffer:** Audio data that will be fixed.

> [!IMPORTANT]
> This method does ***NOT*** work with the integer types.

<br><br><br><br>

```c++
static void Normalize(AudioBuffer& buffer,
                      heph_audio_sample peakAmplitude);
```
Normalizes the audio data at the provided amplitude.
- **buffer:** Audio data that will be normalized.
- **peakAmplitude:** Peak amplitude of the output signal.
<br><br><br><br>

```c++
static void RmsNormalize(AudioBuffer& buffer,
                         heph_float rms);
```
Normalizes the audio data at the provided rms.
- **buffer:** Audio data that will be normalized.
- **rms:** Rms value of the output signal.
<br><br><br><br>

```c++
static void HardClipDistortion(AudioBuffer& buffer,
                               heph_float clippingLevel_dB);
```
Applies hard-clipping distortion.
- **buffer:** Audio data that will be distorted.
- **clippingLevel_dB:** Threshold value in decibels. Must be negative or zero.
<br><br><br><br>

```c++
static void ArctanDistortion(AudioBuffer& buffer,
                             heph_float alpha);
```
Applies soft-clipping distortion via ``arctan`` function.
- **buffer:** Audio data that will be distorted.
- **alpha:** Controls the amount of distortion. Range: ``[0, 1]``.
<br><br><br><br>

```c++
static void CubicDistortion(AudioBuffer& buffer,
                            heph_float a);
```
Applies cubic distortion.
- **buffer:** Audio data that will be distorted.
- **a:** Controls the amount of distortion. Range: ``[0, 1]``.
<br><br><br><br>

```c++
static void Overdrive(AudioBuffer& buffer,
                      heph_float drive);
```
Applies overdrive distortion.
- **buffer:** Audio data that will be distorted.
- **drive:** Controls the amount of distortion. Range: ``[0, 1]``.
<br><br><br><br>

```c++
static void Fuzz(AudioBuffer& buffer,
                 heph_float depth,
                 heph_float alpha);
```
Applies overdrive distortion.
- **buffer:** Audio data that will be distorted.
- **depth:** Controls the dry/wet mix ratio. Range: ``[0, 1]``.
- **alpha:** Controls the amount of distortion. Range: ``[0, 1]``.
<br><br><br><br>

```c++
static void LinearFadeIn(AudioBuffer& buffer,
                         heph_float duration_s);
```
Applies linear fade-in.
- **buffer:** Audio data that the effect will be applied to.
- **duration_s:** Duration of the fade-in in seconds.
<br><br><br><br>

```c++
static void LinearFadeIn(AudioBuffer& buffer,
                         heph_float duration_s,
                         size_t startIndex);
```
Applies linear fade-in.
- **buffer:** Audio data that the effect will be applied to.
- **duration_s:** Duration of the fade-in in seconds.
- **startIndex:** Index of the first frame the effect will be applied to.
<br><br><br><br>

```c++
static void LinearFadeOut(AudioBuffer& buffer,
                          heph_float duration_s);
```
Applies linear fade-out.
- **buffer:** Audio data that the effect will be applied to.
- **duration_s:** Duration of the fade-out in seconds.
<br><br><br><br>

```c++
static void LinearFadeOut(AudioBuffer& buffer,
                          heph_float duration_s,
                          size_t startIndex);
```
Applies linear fade-out.
- **buffer:** Audio data that the effect will be applied to.
- **duration_s:** Duration of the fade-out in seconds.
- **startIndex:** Index of the first frame the effect will be applied to.
<br><br><br><br>

```c++
static void Equalizer(AudioBuffer& buffer,
                      Window& window,
                      const std::vector<EqualizerInfo>& infos);
```
Equalizer.
- **buffer:** Audio data that the effect will be applied to.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
- **infos:** A vector of [EqualizerInfo](/docs/HephAudio/EqualizerInfo.md).
<br><br><br><br>

```c++
static void Equalizer(AudioBuffer& buffer,
                      size_t hopSize,
                      size_t fftSize,
                      Window& window,
                      const std::vector<EqualizerInfo>& infos);
```
Equalizer.
- **buffer:** Audio data that the effect will be applied to.
- **hopSize:** Hop size.
- **fftSize:** Length of the FFT in samples.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
- **infos:** A vector of [EqualizerInfo](/docs/HephAudio/EqualizerInfo.md).
<br><br><br><br>

```c++
static void EqualizerMT(AudioBuffer& buffer,
                        Window& window,
                        const std::vector<EqualizerInfo>& infos);
```
Multithreaded equalizer.
- **buffer:** Audio data that the effect will be applied to.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
- **infos:** A vector of [EqualizerInfo](/docs/HephAudio/EqualizerInfo.md).
<br><br><br><br>

```c++
static void EqualizerMT(AudioBuffer& buffer,
                        size_t threadCountPerChannel,
                        Window& window,
                        const std::vector<EqualizerInfo>& infos);
```
Multithreaded equalizer.
- **buffer:** Audio data that the effect will be applied to.
- **threadCountPerChannel:** Number of threads to use per channel.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
- **infos:** A vector of [EqualizerInfo](/docs/HephAudio/EqualizerInfo.md).
<br><br><br><br>

```c++
static void EqualizerMT(AudioBuffer& buffer,
                        size_t hopSize,
                        size_t fftSize,
                        Window& window,
                        const std::vector<EqualizerInfo>& infos);
```
Multithreaded equalizer.
- **buffer:** Audio data that the effect will be applied to.
- **hopSize:** Hop size.
- **fftSize:** Length of the FFT in samples.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
- **infos:** A vector of [EqualizerInfo](/docs/HephAudio/EqualizerInfo.md).
<br><br><br><br>

```c++
static void EqualizerMT(AudioBuffer& buffer,
                        size_t hopSize,
                        size_t fftSize,
                        size_t threadCountPerChannel,
                        Window& window,
                        const std::vector<EqualizerInfo>& infos);
```
Multithreaded equalizer.
- **buffer:** Audio data that the effect will be applied to.
- **hopSize:** Hop size.
- **fftSize:** Length of the FFT in samples.
- **threadCountPerChannel:** Number of threads to use per channel.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
- **infos:** A vector of [EqualizerInfo](/docs/HephAudio/EqualizerInfo.md).
<br><br><br><br>

```c++
static void ChangeSpeed(AudioBuffer& buffer,
                        heph_float speed,
                        Window& window);
```
Changes the playback speed of the audio data without changing the pitch.
- **buffer:** Audio data that the effect will be applied to.
- **speed:** Playback speed multiplier.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
<br><br><br><br>

```c++
static void ChangeSpeed(AudioBuffer& buffer,
                        size_t hopSize,
                        size_t windowSize,
                        heph_float speed,
                        Window& window);
```
Changes the playback speed of the audio data without changing the pitch.
- **buffer:** Audio data that the effect will be applied to.
- **hopSize:** Hop size.
- **windowSize:** Window size.
- **speed:** Playback speed multiplier.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
<br><br><br><br>

```c++
static void PitchShift(AudioBuffer& buffer,
                       heph_float pitchChange_semitone,
                       Window& window);
```
Changes the pitch of the audio data without changing the playback speed.
- **buffer:** Audio data that the effect will be applied to.
- **pitchChange_semitone:** Change in pitch in terms of semitones.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
<br><br><br><br>

```c++
static void PitchShift(AudioBuffer& buffer,
                       size_t hopSize,
                       size_t fftSize,
                       heph_float pitchChange_semitone,
                       Window& window);
```
Changes the pitch of the audio data without changing the playback speed.
- **buffer:** Audio data that the effect will be applied to.
- **hopSize:** Hop size.
- **fftSize:** Length of the FFT in samples.
- **pitchChange_semitone:** Change in pitch in terms of semitones.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
<br><br><br><br>

```c++
static void PitchShiftMT(AudioBuffer& buffer,
                         heph_float pitchChange_semitone,
                         Window& window);
```
Changes the pitch of the audio data without changing the playback speed (multithreaded).
- **buffer:** Audio data that the effect will be applied to.
- **pitchChange_semitone:** Change in pitch in terms of semitones.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
<br><br><br><br>

```c++
static void PitchShiftMT(AudioBuffer& buffer,
                         heph_float pitchChange_semitone,
                         size_t threadCountPerChannel,
                         Window& window);
```
Changes the pitch of the audio data without changing the playback speed (multithreaded).
- **buffer:** Audio data that the effect will be applied to.
- **pitchChange_semitone:** Change in pitch in terms of semitones.
- **threadCountPerChannel:** Number of threads to use per channel.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
<br><br><br><br>

```c++
static void PitchShiftMT(AudioBuffer& buffer,
                         size_t hopSize,
                         size_t fftSize,
                         heph_float pitchChange_semitone,
                         Window& window);
```
Changes the pitch of the audio data without changing the playback speed (multithreaded).
- **buffer:** Audio data that the effect will be applied to.
- **hopSize:** Hop size.
- **fftSize:** Length of the FFT in samples.
- **pitchChange_semitone:** Change in pitch in terms of semitones.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
<br><br><br><br>

```c++
static void PitchShiftMT(AudioBuffer& buffer,
                         size_t hopSize,
                         size_t fftSize,
                         heph_float pitchChange_semitone,
                         size_t threadCountPerChannel,
                         Window& window);
```
Changes the pitch of the audio data without changing the playback speed (multithreaded).
- **buffer:** Audio data that the effect will be applied to.
- **hopSize:** Hop size.
- **fftSize:** Length of the FFT in samples.
- **pitchChange_semitone:** Change in pitch in terms of semitones.
- **threadCountPerChannel:** Number of threads to use per channel.
- **window:** [Window](/docs/HephAudio/Windows/Window.md) that will be applied to the audio signal.
<br><br><br><br>
