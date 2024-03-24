## AudioBuffer class
```c++
#include <AudioBuffer.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Methods](#methods)


### Description
Class for storing audio data.
<br><br>

### Methods

```c++
AudioBuffer();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
AudioBuffer(size_t frameCount,
            AudioFormatInfo formatInfo);
```
Creates an instance and initializes it with the provided values.
- **frameCount:** Number of audio frames the buffer contains.
- **formatInfo:** Information about the stored audio data.
<br><br><br><br>

```c++
AudioBuffer(size_t frameCount,
            uint16_t channelCount,
            uint32_t sampleRate);
```
Creates an instance and initializes it with the provided values.
- **frameCount:** Number of audio frames the buffer contains.
- **channelCount:** Number of channels. Number of samples that each audio frame contains.
- **sampleRate:** Samples per second (Hz), the number of samples required to represent a one-second-long audio signal.
<br><br><br><br>

```c++
AudioBuffer(std::nullptr_t rhs);
```
Same as the default constructor.
<br><br><br><br>

```c++
AudioBuffer(const AudioBuffer& rhs);
```
Creates an instance and copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
<br><br><br><br>

```c++
AudioBuffer(AudioBuffer&& rhs) noexcept;
```
Creates an instance and moves the ownership of the contents of the ``rhs`` to it.
- **rhs:** The instance whose contents will be moved.
<br><br><br><br>

```c++
~AudioBuffer();
```
Releases the resources.
<br><br><br><br>

```c++
heph_audio_sample* operator[](size_t frameIndex) const;
```
Gets the address of the first sample of the frame at the provided index.
- **frameIndex:** Index of the frame within the buffer.
- **returns:** Address of the first sample of the frame.
<br><br><br><br>

```c++
AudioBuffer operator-() const;
```
Creates a new audio instance that stores the inverted version of the audio signal.
- **returns:** A new buffer that stores the inverted audio signal.
<br><br><br><br>

```c++
AudioBuffer& operator=(std::nullptr_t rhs);
```
Releases the audio data and sets all other fields to their default values.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator=(const AudioBuffer& rhs);
```
Releases the resources of the current buffer, then copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator=(AudioBuffer&& rhs) noexcept;
```
Releases the resources of the current buffer, then moves the ownership of the contents of ``rhs``.
- **rhs:** The instance whose contents will be moved.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer operator+(heph_float rhs) const;
```
Adds a constant to each sample and returns the result in a new instance.
- **rhs:** Constant value that will be added to each sample.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator+(const HephCommon::FloatBuffer& rhs) const;
```
Performs a point-by-point summation and returns the result in a new instance. 
``rhs`` is summed with each channel independently. 
Example: ``ch0 = this_ch0 + rhs; ch1 = this_ch1 + rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator+(const AudioBuffer& rhs) const;
```
Performs a point-by-point summation and returns the result in a new instance.
Each channel are summed independently. 
Example: ``ch0 = this_ch0 + rhs_ch0; ch1 = this_ch1 + rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer& operator+=(heph_float rhs);
```
Adds a constant to each sample and stores the result in the current instance.
- **rhs:** Constant value that will be added to each sample.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator+=(const HephCommon::FloatBuffer& rhs);
```
Performs a point-by-point summation and stores the result in the current instance.
Each channel are summed independently. 
Example: ``this_ch0 += rhs; this_ch1 += rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator+=(const AudioBuffer& rhs);
```
Performs a point-by-point summation and stores the result in the current instance.
Each channel are summed independently. 
Example: ``this_ch0 += rhs_ch0; this_ch1 += rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer operator-(heph_float rhs) const;
```
Subtracts a constant from each sample and returns the result in a new instance.
- **rhs:** Constant value that will be subtracted from each sample.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator-(const HephCommon::FloatBuffer& rhs) const;
```
Performs a point-by-point subtraction and returns the result in a new instance. 
``rhs`` is subtracted from each channel independently. 
Example: ``ch0 = this_ch0 - rhs; ch1 = this_ch1 - rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator-(const AudioBuffer& rhs) const;
```
Performs a point-by-point subtraction and returns the result in a new instance.
Each channel is subtracted independently. 
Example: ``ch0 = this_ch0 - rhs_ch0; ch1 = this_ch1 - rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer& operator-=(heph_float rhs);
```
Subtracts a constant from each sample and stores the result in the current instance.
- **rhs:** Constant value that will be subtracted from each sample.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator-=(const HephCommon::FloatBuffer& rhs);
```
Performs a point-by-point subtraction and stores the result in the current instance.
Each channel is subtracted independently. 
Example: ``this_ch0 -= rhs; this_ch1 -= rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator-=(const AudioBuffer& rhs);
```
Performs a point-by-point subtraction and stores the result in the current instance.
Each channel is subtracted independently. 
Example: ``this_ch0 -= rhs_ch0; this_ch1 -= rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer operator*(heph_float rhs) const;
```
Multiplies a constant with each sample and returns the result in a new instance.
- **rhs:** Constant value that will be multiplied with each sample.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator*(const HephCommon::FloatBuffer& rhs) const;
```
Performs a point-by-point multiplication and returns the result in a new instance. 
``rhs`` is multiplied from each channel independently. 
Example: ``ch0 = this_ch0 * rhs; ch1 = this_ch1 * rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator*(const AudioBuffer& rhs) const;
```
Performs a point-by-point multiplication and returns the result in a new instance.
Each channel is multiplied independently. 
Example: ``ch0 = this_ch0 * rhs_ch0; ch1 = this_ch1 * rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer& operator*=(heph_float rhs);
```
Multiplies a constant from each sample and stores the result in the current instance.
- **rhs:** Constant value that will be multiplied with each sample.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator*=(const HephCommon::FloatBuffer& rhs);
```
Performs a point-by-point multiplication and stores the result in the current instance.
Each channel is multiplied independently. 
Example: ``this_ch0 *= rhs; this_ch1 *= rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator*=(const AudioBuffer& rhs);
```
Performs a point-by-point multiplication and stores the result in the current instance.
Each channel is multiplied independently. 
Example: ``this_ch0 *= rhs_ch0; this_ch1 *= rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer operator/(heph_float rhs) const;
```
Divides each sample by a constant and returns the result in a new instance.
- **rhs:** Constant value that each sample will be divided with.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator/(const HephCommon::FloatBuffer& rhs) const;
```
Performs a point-by-point division and returns the result in a new instance. 
Each channel is divided by ``rhs`` independently. 
Example: ``ch0 = this_ch0 / rhs; ch1 = this_ch1 / rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator/(const AudioBuffer& rhs) const;
```
Performs a point-by-point division and returns the result in a new instance.
Each channel is divided by ``rhs`` independently. 
Example: ``ch0 = this_ch0 / rhs_ch0; ch1 = this_ch1 / rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer& operator/=(heph_float rhs);
```
Divides each sample by a constant and returns the result in the current instance.
- **rhs:** Constant value that each sample will be divided with.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator/=(const HephCommon::FloatBuffer& rhs);
```
Performs a point-by-point division and stores the result in the current instance.
Each channel is divided by ``rhs`` independently. 
Example: ``this_ch0 /= rhs; this_ch1 /= rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator/=(const AudioBuffer& rhs);
```
Performs a point-by-point division and stores the result in the current instance.
Each channel is divided by ``rhs`` independently. 
Example: ``this_ch0 /= rhs_ch0; this_ch1 /= rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>
