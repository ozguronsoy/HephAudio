## TriangleWaveOscillator Class
```c++
#include <Oscillators/TriangleWaveOscillator.h>
using namespace HephAudio;
```
**Inheritance:** *[Oscillator](/docs/HephAudio/Oscillators/Oscillator.md)* -> *TriangleWaveOscillator*

> [Description](#description)<br>
[Methods](#methods)

### Description
Generates a triangle wave signal.
<br><br>

### Methods
```c++
TriangleWaveOscillator();
```
Creates an instance and initializes it with default values.
<br><br><br><br>
```c++
TriangleWaveOscillator(uint32_t sampleRate);
```
Creates an instance and initializes it with provided values.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long triangle wave signal.
<br><br><br><br>
```c++
TriangleWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad);
```
Creates an instance and initializes it with provided values.
- **peakAmplitude:** Maximum amplitude of the triangle wave signal.
- **frequency:** Frequency of the triangle wave signal.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long triangle wave signal.
- **phase_rad:** Phase angle (radian) of the triangle wave signal.
