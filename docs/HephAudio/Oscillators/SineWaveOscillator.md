## SineWaveOscillator Class
**Inheritance:** *[Oscillator](/docs/HephAudio/Oscillators/Oscillator.md)* -> *SineWaveOscillator*

> [Description](#description)<br>
[Methods](#methods)

### Description
Generates a sine wave.
<br><br>

### Methods
```c++
SineWaveOscillator();
```
Creates an instance and initializes it with default values.
<br><br><br><br>
```c++
SineWaveOscillator(uint32_t sampleRate);
```
Creates an instance and initializes it with provided values.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long sine wave signal.
<br><br><br><br>
```c++
SineWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad);
```
Creates an instance and initializes it with provided values.
- **peakAmplitude:** Maximum amplitude of the sine wave signal.
- **frequency:** Frequency of the sine wave signal.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long sine wave signal.
- **phase_rad:** Phase angle (radian) of the sine wave signal.
