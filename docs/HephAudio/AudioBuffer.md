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
Creates a new instance that stores the inverted version of the audio signal.
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
``rhs`` is summed with each channel independently.<br>
Example: ``ch0 = this_ch0 + rhs; ch1 = this_ch1 + rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator+(const AudioBuffer& rhs) const;
```
Performs a point-by-point summation and returns the result in a new instance.
Each channel are summed independently.<br>
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
Each channel are summed independently.<br>
Example: ``this_ch0 += rhs; this_ch1 += rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator+=(const AudioBuffer& rhs);
```
Performs a point-by-point summation and stores the result in the current instance.
Each channel are summed independently.<br>
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
``rhs`` is subtracted from each channel independently.<br>
Example: ``ch0 = this_ch0 - rhs; ch1 = this_ch1 - rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator-(const AudioBuffer& rhs) const;
```
Performs a point-by-point subtraction and returns the result in a new instance.
Each channel is subtracted independently.<br>
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
Each channel is subtracted independently.<br>
Example: ``this_ch0 -= rhs; this_ch1 -= rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator-=(const AudioBuffer& rhs);
```
Performs a point-by-point subtraction and stores the result in the current instance.
Each channel is subtracted independently.<br>
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
``rhs`` is multiplied from each channel independently.<br>
Example: ``ch0 = this_ch0 * rhs; ch1 = this_ch1 * rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator*(const AudioBuffer& rhs) const;
```
Performs a point-by-point multiplication and returns the result in a new instance.
Each channel is multiplied independently.<br>
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
Each channel is multiplied independently.<br>
Example: ``this_ch0 *= rhs; this_ch1 *= rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator*=(const AudioBuffer& rhs);
```
Performs a point-by-point multiplication and stores the result in the current instance.
Each channel is multiplied independently.<br>
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
Each channel is divided by ``rhs`` independently.<br>
Example: ``ch0 = this_ch0 / rhs; ch1 = this_ch1 / rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer operator/(const AudioBuffer& rhs) const;
```
Performs a point-by-point division and returns the result in a new instance.
Each channel is divided by ``rhs`` independently.<br>
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
Each channel is divided by ``rhs`` independently.<br>
Example: ``this_ch0 /= rhs; this_ch1 /= rhs;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer& operator/=(const AudioBuffer& rhs);
```
Performs a point-by-point division and stores the result in the current instance.
Each channel is divided by ``rhs`` independently.<br>
Example: ``this_ch0 /= rhs_ch0; this_ch1 /= rhs_ch1;...``
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer operator<<(size_t rhs) const;
```
Shifts the audio buffer to the left and returns the result in a new instance. 
The frames at the end of the buffer will be set to zero.<br>
Example: for ``rhs = 3``, ``frame_5 -> frame_2``.
- **rhs:** Number of frames to shift.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer& operator<<=(size_t rhs) const;
```
Shifts the audio buffer to the left and returns the result in the current instance. 
The frames at the end of the buffer will be set to zero.<br>
Example: for ``rhs = 3``, ``frame_5 -> frame_2``.
- **rhs:** Number of frames to shift.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
AudioBuffer operator>>(size_t rhs) const;
```
Shifts the audio buffer to the right and returns the result in a new instance. 
The frames at the beginning of the buffer will be set to zero.<br>
Example: for ``rhs = 3``, ``frame_2 -> frame_5``.
- **rhs:** Number of frames to shift.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
AudioBuffer& operator>>=(size_t rhs) const;
```
Shifts the audio buffer to the right and returns the result in the current instance. 
The frames at the beginning of the buffer will be set to zero.<br>
Example: for ``rhs = 3``, ``frame_2 -> frame_5``.
- **rhs:** Number of frames to shift.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
bool operator==(std::nullptr_t rhs) const;
```
Checks whether the buffer is empty.
- **returns:** ``true`` if the buffer is empty, otherwise ``false``.
<br><br><br><br>

```c++
bool operator==(const AudioBuffer& rhs) const;
```
Compares the contents of two buffers.
- **rhs:** Instance that will be compared to.
- **returns:** ``true`` if the contents are equal, otherwise ``false``.
<br><br><br><br>

```c++
bool operator!=(std::nullptr_t rhs) const;
```
Checks whether the buffer is not empty.
- **returns:** ``true`` if the buffer is not empty, otherwise ``false``.
<br><br><br><br>

```c++
bool operator!=(const AudioBuffer& rhs) const;
```
Compares the contents of two buffers.
- **rhs:** Instance that will be compared to.
- **returns:** ``true`` if the contents are not equal, otherwise ``false``.
<br><br><br><br>

```c++
size_t Size() const;
```
Calculates the size, in bytes, of the buffer.
- **returns:** Size of the buffer in bytes.
<br><br><br><br>

```c++
size_t FrameCount() const;
```
Gets the number of audio frames the buffer contains.
- **returns:** Number of audio frames the buffer contains.
<br><br><br><br>

```c++
AudioFormatInfo FrameCount() const;
```
Gets a copy of the audio format info.
- **returns:** Copy of the audio format info.
<br><br><br><br>

```c++
AudioBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
```
Gets the specified part of the buffer.
- **frameIndex:** Index of the first frame of the sub-buffer.
- **frameCount:** Number of frames the sub-buffer will contain.
- **returns:** Sub-buffer.
<br><br><br><br>

```c++
void Append(const AudioBuffer& buffer);
```
Adds the provided audio data at the end of the current buffer.
- **buffer:** Audio data that will be appended.
<br><br><br><br>

```c++
void Insert(const AudioBuffer& buffer, size_t frameIndex);
```
Adds the provided audio data to the provided index of the current buffer.
- **buffer:** Audio data that will be inserted.
- **frameIndex:** Index where the buffer will be inserted.
<br><br><br><br>

```c++
void Cut(size_t frameIndex, size_t frameCount);
```
Removes the specified part of the buffer.
- **frameIndex:** Index of the first frame that will be removed.
- **frameCount:** Number of frames to remove.
<br><br><br><br>

```c++
void Replace(const AudioBuffer& buffer, size_t frameIndex);
```
Replaces the specified part of the buffer with the provided one.
- **buffer:** Audio data that will be added.
- **frameIndex:** Index of the first frame that will be replaced.
<br><br><br><br>

```c++
void Replace(const AudioBuffer& buffer, size_t frameIndex, size_t frameCount);
```
Replaces the specified part of the buffer with the provided one.
- **buffer:** Audio data that will be added.
- **frameIndex:** Index of the first frame that will be replaced.
- **frameCount:** Number of frames to replace.
<br><br><br><br>

```c++
void Reset();
```
Sets the value of all samples to zero.
<br><br><br><br>

```c++
void Resize(size_t newFrameCount);
```
Changes the size of the buffer. 
If the buffer is expanded, new samples will be initialized to zero. 
- **newFrameCount:** New size of the buffer in terms of frames.
<br><br><br><br>

```c++
void Empty();
```
Releases the audio data and sets all other fields to their default values.
<br><br><br><br>

```c++
heph_audio_sample Min() const;
```
Gets the sample with the minimum value.
- **returns:** Sample with the minimum value.
<br><br><br><br>

```c++
heph_audio_sample Max() const;
```
Gets the sample with the maximum value.
- **returns:** Sample with the maximum value.
<br><br><br><br>

```c++
heph_audio_sample AbsMax() const;
```
Gets the sample with the maximum absolute value.
- **returns:** Sample with the maximum absolute value.
<br><br><br><br>

```c++
heph_float Rms() const;
```
Calculates the rms value of the buffer.
- **returns:** Rms value.
<br><br><br><br>

```c++
AudioBuffer Convolve(const HephCommon::FloatBuffer& h) const;
```
Calculates the convolution.
- **h:** rhs.
- **returns:** Convolution result.
<br><br><br><br>

```c++
AudioBuffer Convolve(const HephCommon::FloatBuffer& h, HephCommon::ConvolutionMode convolutionMode) const;
```
Calculates the convolution.
- **h:** rhs.
- **convolutionMode:** Convolution mode.
- **returns:** Convolution result.
<br><br><br><br>

```c++
AudioBuffer Convolve(const AudioBuffer& h) const;
```
Calculates the convolution.
- **h:** rhs.
- **returns:** Convolution result.
<br><br><br><br>

```c++
AudioBuffer Convolve(const AudioBuffer& h, HephCommon::ConvolutionMode convolutionMode) const;
```
Calculates the convolution.
- **h:** rhs.
- **convolutionMode:** Convolution mode.
- **returns:** Convolution result.
<br><br><br><br>

```c++
heph_float CalculateDuration() const;
```
Calculates the duration of the buffers in terms of seconds.
- **returns:** Duration of the buffer in seconds.
<br><br><br><br>

```c++
size_t CalculateFrameIndex(heph_float t_s) const;
```
Calculates the frame index the provided time corresponds to.
- **t_s:** Time in seconds.
- **returns:** Frame index.
<br><br><br><br>

```c++
void* Begin() const;
```
Gets the address of the first sample of the first frame.
- **returns:** Address of the first sample if there are any, otherwise ``nullptr``.
<br><br><br><br>

```c++
void* End() const;
```
Gets the address of the end of the buffer.
- **returns:** Address of the end of the buffer.
<br><br><br><br>

```c++
static heph_float CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo);
```
Calculates the duration in terms of seconds.
- **frameCount:** Number of frames.
- **formatInfo:** Format info that will be used to calculate the duration.
- **returns:** Duration in seconds.
<br><br><br><br>

```c++
static size_t CalculateFrameIndex(heph_float t_s, AudioFormatInfo formatInfo);
```
Calculates the frame index the provided time corresponds to.
- **t_s:** Time in seconds.
- **formatInfo:** Format info that will be used to calculate the frame index.
- **returns:** Index of the frame the provided time corresponds to.
<br><br><br><br>
