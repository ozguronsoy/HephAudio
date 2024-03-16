## WinAudioBase class
```c++
#include <NativeAudio/WinAudioBase.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *WinAudioBase*


> [Description](#description)<br>
[Methods](#methods)<br>
[Protected Methods](#protected_methods)

### Description
Base class for the classes that interact with the windows native audio APIs.
<br><br>

### Methods

```c++
WinAudioBase();
```
Creates an instance and initializes it with default values.
<br><br><br><br>

```c++
virtual ~WinAudioBase() = default;
```
Default virtual destructor.
<br><br><br><br>

### Protected Methods
```c++
void InitializeCOM() const;
```
Initializes the COM as multithreaded if not already initialized as apartmentthreaded.
<br><br><br><br>

```c++
static AudioFormatInfo WFX2AFI(const WAVEFORMATEX& wfx);
```
Converts [WAVEFORMATEX](https://learn.microsoft.com/en-us/windows/win32/api/mmeapi/ns-mmeapi-waveformatex) struct to [AudioFormatInfo](/docs/HephAudio/AudioFormatInfo.md) struct.
- **wfx:** WAVEFORMATEX struct.
- **returns:** AudioFormatInfo struct.
<br><br><br><br>

```c++
static WAVEFORMATEX AFI2WFX(const AudioFormatInfo& afi);
```
Converts [AudioFormatInfo](/docs/HephAudio/AudioFormatInfo.md) struct to [WAVEFORMATEX](https://learn.microsoft.com/en-us/windows/win32/api/mmeapi/ns-mmeapi-waveformatex) struct.
- **afi:** AudioFormatInfo struct.
- **returns:** WAVEFORMATEX struct.
