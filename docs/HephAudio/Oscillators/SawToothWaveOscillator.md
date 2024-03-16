## SawToothWaveOscillator Class
```c++
#include <Oscillators/SawToothWaveOscillator.h>
using namespace HephAudio;
```
**Inheritance:** *[Oscillator](/docs/HephAudio/Oscillators/Oscillator.md)* -> *SawToothWaveOscillator*

> [Description](#description)<br>
[Methods](#methods)

### Description
Generates a sawtooth wave signal.
<br><br>

### Methods
```c++
SawToothWaveOscillator();
```
Creates an instance and initializes it with default values.
<br><br><br><br>
```c++
SawToothWaveOscillator(uint32_t sampleRate);
```
Creates an instance and initializes it with provided values.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long sawtooth wave signal.
<br><br><br><br>
```c++
SawToothWaveOscillator(heph_float peakAmplitude, heph_float frequency, uint32_t sampleRate, heph_float phase_rad);
```
Creates an instance and initializes it with provided values.
- **peakAmplitude:** Maximum amplitude of the sawtooth wave signal.
- **frequency:** Frequency of the sawtooth wave signal.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long sawtooth wave signal.
- **phase_rad:** Phase angle (radian) of the sawtooth wave signal.
