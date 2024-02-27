# Using Oscillators

Oscillators are used when we need variables that change periodically. 
You may have encountered the word **LFO** while playing with some sound effects controls. 
LFO means "Low-Frequency Oscillator" and LFOs are commonly used in modulation effects such as chorus, tremolo, and flanger. 
To better understand how we can utilize oscillators, let's write a tremolo effect.
<br><br><br><br>

### Custom Tremolo Effect

The tremolo effect is achieved by changing the volume of an audio signal periodically.<br>
Since we have a parameter that changes periodically, we can use the [oscillator](/docs/HephAudio/Oscillators/) classes to obtain this parameter.<br>
Our method will take an [AudioBuffer](/docs/HephAudio/AudioBuffer.md) reference and an [Oscillator](/docs/HephAudio/Oscillators/Oscillator.md) reference as input. 
```c++
void MyTremolo(AudioBuffer& buffer, const Oscillator& lfo);
```
The shape of the oscillator directly affects the resulting sound. Hence, we use the base oscillator class to avoid restricting the user to a single shape.
```c++
void MyTremolo(AudioBuffer& buffer, const Oscillator& lfo)
{
    for (size_t i = 0; i < buffer.FrameCount(); i++)
    {
        const heph_float volume = lfo[i]; // calculate the volume and use it for all channels.
        for (size_t j = 0; j < buffer.FormatInfo().channelCount; j++)
        {
            buffer[i][j] *= volume;
        }
    }
}
```
We created a simple tremolo effect. We can improve it by mixing the original input signal (dry) and the output signal we just calculated (wet). 
That will make our effect even more customizable and increase the variaty of sounds we can achieve.
We will take another parameter to control the dry/wet ratio and call it *depth*. This parameter will be between 0 and 1.<br>
For *depth = 0*, the output will only contain the dry signal, and vice versa for *depth = 1*.
```c++
void MyTremolo(AudioBuffer& buffer, heph_float depth, const Oscillator& lfo)
{
    const heph_float wetFactor = depth;
    const heph_float dryFactor = 1.0 - wetFactor;

    for (size_t i = 0; i < buffer.FrameCount(); i++)
    {
        const heph_float volume = lfo[i]; // calculate the volume and use it for all channels.
        for (size_t j = 0; j < buffer.FormatInfo().channelCount; j++)
        {
            const heph_float drySample = buffer[i][j];
            const heph_float wetSample = drySample * volume;
            buffer[i][j] = drySample * dryFactor + wetSample * wetFactor;
        }
    }
}
```
Test it using [SineWaveOscillator](/docs/HephAudio/Oscillators/SineWaveOscillator.md).
```c++
#include <iostream>
#include <Audio.h>
#include <SineWaveOscillator.h>

using namespace HephAudio;

void MyTremolo(AudioBuffer& buffer, heph_float depth, const Oscillator& lfo);

int main()
{
    Audio audio;
    audio.InitializeRender(2, 48000);
    AudioObject* pAudioObject = audio.Load("some_path\\some_file.wav");

    heph_float depth = 0.7;
    uint32_t bufferSampleRate = pAudioObject->buffer.FormatInfo().sampleRate;
    SineWaveOscillator lfo(1.0, 8.0, bufferSampleRate, 0); // create low-frequency sine wave oscillator
    MyTremolo(pAudioObject->buffer, depth, lfo);

    pAudioObject->isPaused = false;

    // stop from exiting the program.
    std::string s;
    std::cin >> s;

    return 0;
}

void MyTremolo(AudioBuffer& buffer, heph_float depth, const Oscillator& lfo)
{
    const heph_float wetFactor = depth;
    const heph_float dryFactor = 1.0 - wetFactor;

    for (size_t i = 0; i < buffer.FrameCount(); i++)
    {
        const heph_float volume = lfo[i]; // calculate the volume and use it for all channels.
        for (size_t j = 0; j < buffer.FormatInfo().channelCount; j++)
        {
            const heph_float drySample = buffer[i][j];
            const heph_float wetSample = drySample * volume;
            buffer[i][j] = drySample * dryFactor + wetSample * wetFactor;
        }
    }
}
```
