## NativeAudio class
```c++
#include <NativeAudio/NativeAudio.h>
using namespace HephAudio::Native;
```

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)<br>
[Protected Fields](#protected-fields)<br>
[Protected Methods](#protected-methods)

### Description
Base class for the classes that interact with the native audio APIs.
<br><br>

### Fields
```c++
class NativeAudio
{
	HephCommon::Event OnAudioDeviceAdded;
	HephCommon::Event OnAudioDeviceRemoved;
	HephCommon::Event OnCapture;
}
```

- ``OnAudioDeviceAdded``
<br><br>
The event that will be invoked every time an audio device is enabled, connected, or became available for any other reason.
<br><br>

- ``OnAudioDeviceRemoved``
<br><br>
The event that will be invoked every time an audio device is disabled, disconnected, or became unavailable for any other reason.
<br><br>

- ``OnCapture``
<br><br>
The event that will be invoked every time new audio data is captured.
<br><br>

### Methods
See the [Audio](/docs/HephAudio/Audio.md#methods) class for the method descriptions.

```c++
NativeAudio();
```
Creates an instance and initializes it with default values.
<br><br><br><br>

```c++
virtual ~NativeAudio() = default;
```
Releases the resources.
<br><br><br><br>


### Protected Fields

```c++
class NativeAudio
{
	static constexpr bool DEVICE_ENUMERATION_FAIL = false;
	static constexpr bool DEVICE_ENUMERATION_SUCCESS = true;

	std::vector<AudioObject> audioObjects;
	std::vector<AudioDevice> audioDevices;
	std::thread::id mainThreadId;
	std::thread renderThread;
	std::thread captureThread;
	std::thread deviceThread;
	HephCommon::StringBuffer renderDeviceId;
	HephCommon::StringBuffer captureDeviceId;
	AudioFormatInfo renderFormat;
	AudioFormatInfo captureFormat;
	bool disposing;
	bool isRenderInitialized;
	bool isCaptureInitialized;
	bool isCapturePaused;
	uint32_t deviceEnumerationPeriod_ms;
	mutable std::mutex audioDevicesMutex;
	mutable std::recursive_mutex audioObjectsMutex;
}
```

- ``DEVICE_ENUMERATION_FAIL``
<br><br>
Indicates that the device enumeration has failed.
<br><br>

- ``DEVICE_ENUMERATION_SUCCESS``
<br><br>
Indicates that the device enumeration has succeeded.
<br><br>

- ``audioObjects``
<br><br>
Internal vector of [AudioObject](/docs/HephAudio/AudioObject.md) instances created by one of the ``Play``, ``Loop``, or ``CreateAudioObject`` methods.
<br><br>

- ``audioDevices``
<br><br>
Vector of available audio devices. Emptied and refilled every time the ``EnumerateAudioDevices`` method is called.
<br><br>

- ``mainThreadId``
<br><br>
The unique id of the thread where the ``NativeAudio`` class is instantiated.
<br><br>

- ``renderThread``
<br><br>
The thread that handles rendering.
<br><br>

- ``captureThread``
<br><br>
The thread that handles capturing.
<br><br>

- ``deviceThread``
<br><br>
The thread that handles device enumeration.
<br><br>

- ``renderDeviceId``
<br><br>
The unique id of the audio device that's currently being used for rendering.
<br><br>

- ``captureDeviceId``
<br><br>
The unique id of the audio device that's currently being used for capturing.
<br><br>

- ``renderFormat``
<br><br>
The format of the rendered audio data.
<br><br>

- ``captureFormat``
<br><br>
The format of the captured audio data.
<br><br>

- ``disposing``
<br><br>
Indicates whether the ``NativeAudio`` instance is being destructed.
<br><br>

- ``isRenderInitialized``
<br><br>
Indicates whether the native API is initailzed and ready for rendering audio data.
<br><br>

- ``isCaptureInitialized``
<br><br>
Indicates whether the native API is initailzed and ready for capturing audio data.
<br><br>

- ``isCapturePaused``
<br><br>
Indicates whether to pause capturing audio data.
<br><br>

- ``deviceEnumerationPeriod_ms``
<br><br>
Time waited between each call to the ``EnumerateAudioDevices`` method.
<br><br>

- ``audioDevicesMutex``
<br><br>
Mutex for preventing race conditions during device enumeration.
<br><br>

- ``audioObjectsMutex``
<br><br>
Mutex for preventing race conditions during rendering.
<br><br>


### Protected Methods

```c++
virtual bool EnumerateAudioDevices() = 0;
```
Implements the device enumeration by interacting with the native APIs.
- **returns:** ``true`` if the device enumeration had succeeded, otherwise ``false``.
<br><br><br><br>

```c++
virtual void CheckAudioDevices();
```
Calls the ``EnumerateAudioDevices`` method periodically to check if any new devices have been connected, or old devices have been disconnected, since the last call.
<br><br><br><br>

```c++
void JoinRenderThread();
```
Waits for the render thread to join. 
You ***MUST*** set either one of the ``isRenderInitialized`` or ``disposing`` fields to ``true`` prior to calling this method to signal the render thread to stop.
<br><br><br><br>

```c++
void JoinCaptureThread();
```
Waits for the render thread to join. 
You ***MUST*** set either one of the ``isCaptureInitialized`` or ``disposing`` fields to ``true`` prior to calling this method to signal the capture thread to stop.
<br><br><br><br>

```c++
void JoinDeviceThread();
```
Waits for the render thread to join. 
You ***MUST*** set the ``disposing`` field to ``true`` prior to calling this method to signal the device thread to stop.
<br><br><br><br>

```c++
void Mix(AudioBuffer& outputBuffer, uint32_t frameCount);
```
Mixes the [AudioObjects](/docs/HephAudio/AudioObject.md) that are currently playing to a single buffer before rendering it.
- **outputBuffer:** The buffer that will contain the mixed audio data.
- **frameCount:** Number of audio frames that will be rendered.
<br><br><br><br>

```c++
size_t GetAOCountToMix() const;
```
Gets the number of [AudioObjects](/docs/HephAudio/AudioObject.md) that will be mixed.
- **returns:** Number of audio objects that will be mixed.
<br><br><br><br>

```c++
virtual heph_float GetFinalAOVolume(AudioObject* pAudioObject) const;
```
Calculates the volume multiplier for the provided object. 
Typically returns the ``pAudioObject->volume`` field. 
This method is used when the native API does not support changing the master volume, so you need to implement it.
- **pAudioObject:** Object whose volume multiplier will be calculated.
- **returns:** The volume multiplier.
<br><br><br><br>
