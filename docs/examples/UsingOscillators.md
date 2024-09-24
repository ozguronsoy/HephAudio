# Using Oscillators

Oscillators are used when we need variables that change periodically. 
You may have encountered the word **LFO** while playing with a sound effect's controls. 
LFO means "Low-Frequency Oscillator" and LFOs are commonly used in modulation effects such as chorus, tremolo, and flanger. 
To better understand how we can utilize oscillators, let's write a tremolo effect.
<br><br><br><br>

### Custom Tremolo Effect

The tremolo effect is achieved by changing the volume of an audio signal periodically.<br>
Since we have a parameter that changes periodically, we can use the oscillator classes to obtain this parameter.<br>

```c++
void MyTremolo(AudioBuffer& buffer, const Oscillator& lfo)
{
    for (size_t i = 0; i < buffer.FrameCount(); i++)
    {
        const double volume = lfo[i]; 
        for (size_t j = 0; j < buffer.FormatInfo().channelLayout.count; j++)
        {
            buffer[i][j] *= volume;
        }
    }
}
```

We created a simple tremolo effect. We can improve it by mixing the original input signal (dry) and the output signal we just computed (wet). 
That will make our effect even more customizable and increase the variety of sounds we can achieve.
We will take another parameter to control the dry/wet ratio and call it *depth*. This parameter will be between 0 and 1.<br>
For *depth = 0*, the output will only contain the dry signal, and vice versa.

```c++
void MyTremolo(AudioBuffer& buffer, double depth, const Oscillator& lfo)
{
    const double wetFactor = depth;
    const double dryFactor = 1.0 - wetFactor;

    for (size_t i = 0; i < buffer.FrameCount(); i++)
    {
        const double volume = lfo[i]; 
        for (size_t j = 0; j < buffer.FormatInfo().channelLayout.count; j++)
        {
            const double drySample = buffer[i][j];
            const double wetSample = drySample * volume;
            buffer[i][j] = drySample * dryFactor + wetSample * wetFactor;
        }
    }
}
```
Test it using ``SineWaveOscillator``
```c++
#include <iostream>
#include <Audio.h>
#include <Oscillators/SineWaveOscillator.h>

using namespace HephAudio;

void MyTremolo(AudioBuffer& buffer, double depth, const Oscillator& lfo);

int main()
{
    Audio audio;
    audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
    AudioObject* pAudioObject = audio.Load("some_path\\some_file.wav");

    double depth = 0.7;
    
    uint32_t bufferSampleRate = pAudioObject->buffer.FormatInfo().sampleRate;
    
    SineWaveOscillator lfo(1.0, 8.0, bufferSampleRate, 0); 
    
    MyTremolo(pAudioObject->buffer, depth, lfo);

    pAudioObject->OnRender = HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT;
    pAudioObject->isPaused = false;

    // stop from exiting the program.
    std::string s;
    std::cin >> s;

    return 0;
}

void MyTremolo(AudioBuffer& buffer, double depth, const Oscillator& lfo)
{
    const double wetFactor = depth;
    const double dryFactor = 1.0 - wetFactor;

    for (size_t i = 0; i < buffer.FrameCount(); i++)
    {
        const double volume = lfo[i];
        for (size_t j = 0; j < buffer.FormatInfo().channelLayout.count; j++)
        {
            const double drySample = buffer[i][j];
            const double wetSample = drySample * volume;
            buffer[i][j] = drySample * dryFactor + wetSample * wetFactor;
        }
    }
}
```
