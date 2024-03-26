## Fourier class
```c++
#include <Fourier.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Methods](#methods)



### Description
Class for calculating the Fourier transform.

> [!NOTE]
> This class only contains static methods and cannot be instantiated.

<br><br>

### Methods

```c++
static ComplexBuffer FFT(const FloatBuffer& floatBuffer);
```
Calculates the FFT.
- **floatBuffer:** Real data in time domain.
- **returns:** Complex data in frequency domain.
<br><br><br><br>

```c++
static ComplexBuffer FFT(const FloatBuffer& floatBuffer,
                         size_t fftSize);
```
Calculates the FFT.
- **floatBuffer:** Real data in time domain.
- **fftSize:** FFT size, must be a power of 2.
- **returns:** Complex data in frequency domain.
<br><br><br><br>

```c++
static void FFT(ComplexBuffer& complexBuffer);
```
Calculates the FFT.
- **complexBuffer:** Complex data in time domain.
<br><br><br><br>

```c++
static void FFT(ComplexBuffer& complexBuffer,
                size_t fftSize);
```
Calculates the FFT.
- **complexBuffer:** Complex data in time domain.
- **fftSize:** FFT size, must be a power of 2.
<br><br><br><br>

```c++
static void IFFT(FloatBuffer& floatBuffer,
                 ComplexBuffer& complexBuffer);
```
Calculates the inverse FFT.
- **floatBuffer:** Destination buffer for storing the real data in time domain.
- **complexBuffer:** Complex data in frequency domain.
<br><br><br><br>

```c++
static void IFFT(ComplexBuffer& complexBuffer,
                 bool scale);
```
Calculates the inverse FFT.
- **complexBuffer:** Complex data in frequency domain.
- **scale:** Indicates whether to multiply the output by ``1 / fftSize``.
<br><br><br><br>

```c++
static heph_float BinFrequencyToIndex(size_t sampleRate,
                                      size_t fftSize,
                                      heph_float frequency);
```
Calculates the bin index corresponding to the provided frequency.
- **sampleRate:** Sample rate in Hz.
- **fftSize:** FFT size.
- **frequency:** Frequency in Hz.
- **returns:** Bin index.
<br><br><br><br>

```c++
static heph_float IndexToBinFrequency(size_t sampleRate,
                                      size_t fftSize,
                                      size_t index);
```
Calculates the frequency corresponding to the provided bin index.
- **sampleRate:** Sample rate in Hz.
- **fftSize:** FFT size.
- **index:** Bin index.
- **returns:** Frequency in Hz.
<br><br><br><br>

```c++
static size_t CalculateFFTSize(size_t bufferSize);
```
Rounds the provided size to the nearest power of 2.
- **bufferSize:** Desired FFT size.
- **returns:** FFT size.
<br><br><br><br>

```c++
static FloatBuffer Convolve(const FloatBuffer& source,
                            const FloatBuffer& kernel);
```
Calculates the convolution via FFT.
- **source:** Left hand side.
- **kernel:** Right hand side.
- **returns:** Convolution result.
<br><br><br><br>

```c++
static FloatBuffer Convolve(const FloatBuffer& source,
                            const FloatBuffer& kernel,
                            ConvolutionMode convolutionMode);
```
Calculates the convolution via FFT.
- **source:** Left hand side.
- **kernel:** Right hand side.
- **convolutionMode:** Convolution mode.
- **returns:** Convolution result.
<br><br><br><br>
