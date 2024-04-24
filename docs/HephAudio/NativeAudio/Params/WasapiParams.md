## WasapiParams struct
```c++
#include <NativeAudio/Params/WasapiParams.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudioParams](/docs/HephAudio/NativeAudio/Params/NativeAudioParams.md)* -> *WasapiParams*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)

### Description
Struct for storing the WASAPI specific parameters.
<br><br>

### Fields

```c++
struct WasapiParams final : public NativeAudioParams
{
	tagCLSCTX renderClsCtx;
	tagCLSCTX captureClsCtx;
	AUDCLNT_SHAREMODE renderShareMode;
	AUDCLNT_SHAREMODE captureShareMode;
	DWORD renderStreamFlags;
	DWORD captureStreamFlags;
	heph_float renderBufferDuration_ms;
	heph_float captureBufferDuration_ms;
	heph_float renderPeriodicity_ms;
	heph_float capturePeriodicity_ms;
};
```

- ``renderClsCtx``
<br><br>
[render context](https://learn.microsoft.com/en-us/windows/win32/api/wtypesbase/ne-wtypesbase-clsctx).
<br><br>

- ``captureClsCtx``
<br><br>
[capture context](https://learn.microsoft.com/en-us/windows/win32/api/wtypesbase/ne-wtypesbase-clsctx).
<br><br>

- ``renderShareMode``
<br><br>
[Render share mode](https://learn.microsoft.com/en-us/windows/win32/api/audiosessiontypes/ne-audiosessiontypes-audclnt_sharemode), ``AUDCLNT_SHAREMODE_SHARED`` or ``AUDCLNT_SHAREMODE_EXCLUSIVE``.
<br><br>

- ``captureShareMode``
<br><br>
[Capture share mode](https://learn.microsoft.com/en-us/windows/win32/api/audiosessiontypes/ne-audiosessiontypes-audclnt_sharemode), ``AUDCLNT_SHAREMODE_SHARED`` or ``AUDCLNT_SHAREMODE_EXCLUSIVE``.
<br><br>

- ``renderStreamFlags``
<br><br>
[Render stream flags](https://learn.microsoft.com/en-us/windows/win32/coreaudio/audclnt-streamflags-xxx-constants).
<br><br>

- ``captureStreamFlags``
<br><br>
[Capture stream flags](https://learn.microsoft.com/en-us/windows/win32/coreaudio/audclnt-streamflags-xxx-constants).
<br><br>

- ``renderBufferDuration_ms``
<br><br>
Duration of the render buffer in milliseconds.
<br><br>

- ``captureBufferDuration_ms``
<br><br>
Duration of the capture buffer in milliseconds.
<br><br>

- ``renderPeriodicity_ms``
<br><br>
Periodicity of the render device in milliseconds. This must be equal to the render buffer duration for exclusive streams.
<br><br>

- ``capturePeriodicity_ms``
<br><br>
Periodicity of the capture device in milliseconds. This must be equal to the capture buffer duration for exclusive streams.
<br><br>


### Methods

```c++
WasapiParams();
```
Creates an instance and initializes it with default values.
