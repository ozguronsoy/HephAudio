## Oscillator Class

> [Description](#description)<br>
[Parameters](#parameters)<br>
[Methods](#methods)

### Description
Base class for oscillators. Oscillators are used for generating periodic signals. 
Check this [tutorial](/docs/tutorials/UsingOscillators.md) to learn how and when to use oscillators.
<br><br>

### Parameters
```c++
class Oscillator
{
    heph_float peakAmplitude;
    heph_float frequency;
    heph_float phase_rad;
    uint32_t sampleRate;
};
```

- ``peakAmplitude``
<br><br>
Maximum amplitude of the periodic signal.
<br><br>
- ``frequency``
<br><br>
Frequency of the periodic signal.
<br><br>
- ``phase_rad``
<br><br>
Phase angle (radian) of the periodic signal.
<br><br>
- ``sampleRate``
<br><br>
Samples per second (Hz), the number of samples required to represent a one-second-long periodic signal.
<br><br>

### Methods
```c++
virtual ~Oscillator() = default;
```
Default virtual destructor.
<br><br><br><br>
```c++
virtual heph_float operator[](size_t n) const = 0;
```
Calculates the sample at index **n**.
- **n:** Index of the sample.
- **Returns:** The sample calculated at the nth index.
<br><br><br><br>
```c++
HephCommon::FloatBuffer GenerateBuffer() const;
```
Generates a one-period-long buffer.
- **Returns:** The generated buffer.
<br><br><br><br>
```c++
HephCommon::FloatBuffer GenerateBuffer(size_t frameCount) const;
```
Generates a buffer with the provided size.
- **frameCount:** Size of the buffer in frames.
- **Returns:** The generated buffer.
<br><br><br><br>

```c++
HephCommon::FloatBuffer GenerateBuffer(size_t frameIndex, size_t frameCount) const;
```
Generates a buffer with the provided size starting at the provided index.
- **frameIndex:** Index of the first sample.
- **frameCount:** Size of the buffer in frames.
- **Returns:** The generated buffer.