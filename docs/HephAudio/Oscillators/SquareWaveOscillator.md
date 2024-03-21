## SquareWaveOscillator Class
```c++
#include <Oscillators/SquareWaveOscillator.h>
using namespace HephAudio;
```
**Inheritance:** *[Oscillator](/docs/HephAudio/Oscillators/Oscillator.md)* -> *SquareWaveOscillator*

> [Description](#description)<br>
[Methods](#methods)

### Description
Generates a square wave signal.
<br><br>

### Methods
```c++
SquareWaveOscillator();
```
Creates an instance and initializes it with default values.
<br><br><br><br>
```c++
SquareWaveOscillator(uint32_t sampleRate);
```
Creates an instance and initializes it with provided values.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long square wave signal.
<br><br><br><br>
```c++
SquareWaveOscillator(heph_float peakAmplitude,
                     heph_float frequency,
                     uint32_t sampleRate,
                     heph_float phase_rad);
```
Creates an instance and initializes it with provided values.
- **peakAmplitude:** Maximum amplitude of the square wave signal.
- **frequency:** Frequency of the square wave signal.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long square wave signal.
- **phase_rad:** Phase angle (radian) of the square wave signal.
