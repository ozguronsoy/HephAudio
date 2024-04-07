## Audio class
```c++
#include <Audio.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Enums](#enums)<br>
[Methods](#methods)


### Description
Class for playing and recording audio using the native APIs.

<br><br>

### Enums
```c++
enum AudioAPI
{
    Default
};
```
Defines constants that indicate which native API is going to be used internally by the Audio class.<br>
If you are unsure which API to use, select the ``Default`` constant.

> [!NOTE]
> The defined constants change accordingly to the targeted platform except the ``Default`` constant.

<br><br>

### Methods
```c++
Audio();
```
Creates an instance and initializes it with default values. Uses the newest available native API for the current platform.
<br><br><br><br>

```c++
Audio(AudioAPI api);
```
Creates an instance and initializes it with default values. Uses the native API requested by the user.
- **api:** The native API which will be used.
<br><br><br><br>

```c++
~Audio();
```
Releases the resources.
<br><br><br><br>

```c++
Native::NativeAudio* GetNativeAudio() const;
```
Gets the pointer to the [NativeAudio]((/docs/HephAudio/NativeAudio/NativeAudio.md)) instance that's internally used.
- **returns:** Pointer to the NativeAudio instance that's internally used.
<br><br><br><br>

```c++
AudioObject* Play(const std::string& filePath);
```
Reads the file, then starts playing it.
- **filePath:** Path of the file which will be played.
- **returns:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance.
<br><br><br><br>

```c++
AudioObject* Play(const std::string& filePath,
                  uint32_t playCount);
```
Reads the file, then starts playing it.
- **filePath:** Path of the file which will be played.
- **playCount:** Number of times the file will be played from the beginning.
- **returns:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance.
<br><br><br><br>

```c++
AudioObject* Play(const std::string& filePath,
                  uint32_t playCount,
                  bool isPaused);
```
Reads the file, then starts playing it if the ``isPaused`` parameter is ``false``.
- **filePath:** Path of the file which will be played.
- **playCount:** Number of times the file will be played from the beginning.
- **isPaused:** Indicates whether to start playing the file after it's read.
- **returns:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance.
<br><br><br><br>

```c++
AudioObject* Load(const std::string& filePath);
```
Reads the file, then converts the audio data to the render format but does not start playing it.
- **filePath:** Path of the file which will be loaded.
- **returns:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance.
<br><br><br><br>

```c++
AudioObject* Load(const std::string& filePath,
                  uint32_t playCount);
```
Reads the file, then converts the audio data to the render format but does not start playing it.
- **filePath:** Path of the file which will be loaded.
- **playCount:** Number of times the file will be played from the beginning.
- **returns:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance.
<br><br><br><br>

```c++
AudioObject* Load(const std::string& filePath,
                  uint32_t playCount,
                  bool isPaused);
```
Reads the file, then converts the audio data to the render format, then starts playing it if the ``isPaused`` parameter is ``false``.
- **filePath:** Path of the file which will be loaded.
- **playCount:** Number of times the file will be played from the beginning.
- **isPaused:** Indicates whether to start playing the file after it's read and converted.
- **returns:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance.
<br><br><br><br>

```c++
AudioObject* CreateAudioObject(const std::string& name,
                               size_t bufferFrameCount,
                               AudioFormatInfo bufferFormatInfo);
```
Creates an [AudioObject](/docs/HephAudio/AudioObject.md) instance with the provided buffer info. 
This method is usefull when you want to play audio data that is not obtained by reading a file. 
Or you want to play a file but only decode the data which is about to be rendered, which the [AudioStream](/docs/HephAudio/AudioStream.md) class does.
- **name:** A name for the object. Does not have to be unique.
- **bufferFrameCount:** Number of frames the buffer will have.
- **bufferFormatInfo:** The format of the buffer.
- **returns:** Pointer to the [AudioObject](/docs/HephAudio/AudioObject.md) instance.
<br><br><br><br>

```c++
bool DestroyAudioObject(AudioObject* pAudioObject);
```
Destroyes the [AudioObject](/docs/HephAudio/AudioObject.md) created by one of the ``Play``, ``Load``, or ``CreateAudioObject`` methods.
- **pAudioObject:** Pointer to the object which will be destroyed.
- **returns:** ``true`` if the object is found and destroyed, otherwise ``false``.
<br><br><br><br>

```c++
bool DestroyAudioObject(const HephCommon::Guid& audioObjectId);
```
Destroyes the [AudioObject](/docs/HephAudio/AudioObject.md) created by one of the ``Play``, ``Load``, or ``CreateAudioObject`` methods.
- **audioObjectId:** Unique id of the object which will be destroyed.
- **returns:** ``true`` if the object is found and destroyed, otherwise ``false``.
<br><br><br><br>

```c++
bool AudioObjectExists(AudioObject* pAudioObject) const;
```
Checks whether an [AudioObject](/docs/HephAudio/AudioObject.md) exists at the provided memory address.
- **pAudioObject:** Pointer to the memory address which will be checked.
- **returns:** ``true`` if the object is found, otherwise ``false``.
<br><br><br><br>

```c++
bool AudioObjectExists(const HephCommon::Guid& audioObjectId) const;
```
Checks whether an [AudioObject](/docs/HephAudio/AudioObject.md) exists with the provided id.
- **audioObjectId:** Unique id which will be checked.
- **returns:** ``true`` if the object is found, otherwise ``false``.
<br><br><br><br>

```c++
AudioObject* GetAudioObject(size_t index) const;
```
Gets the [AudioObject](/docs/HephAudio/AudioObject.md) at the provided index.
- **index:** Index of the audio object.
- **returns:** The audio object if found, otherwise ``nullptr``.
<br><br><br><br>

```c++
AudioObject* GetAudioObject(const HephCommon::Guid& audioObjectId);
```
Gets the [AudioObject](/docs/HephAudio/AudioObject.md) with the provided id.
- **audioObjectId:** Unique id of the audio object.
- **returns:** The audio object if found, otherwise ``nullptr``.
<br><br><br><br>

```c++
AudioObject* GetAudioObject(const std::string& audioObjectName) const;
```
Gets the first [AudioObject](/docs/HephAudio/AudioObject.md) with the provided name.
- **audioObjectName:** Name of the audio object.
- **returns:** The first audio object that has the provided name if found, otherwise ``nullptr``.
<br><br><br><br>

```c++
size_t GetAudioObjectCount() const;
```
Gets the number of audio objects that are created by one of the ``Play``, ``Load``, or ``CreateAudioObject`` methods.
- **returns:** The number of audio objects.
<br><br><br><br>

```c++
void ResumeCapture();
```
Resumes capturing.
<br><br><br><br>

```c++
void PauseCapture();
```
Pauses capturing.
<br><br><br><br>

```c++
bool IsCapturePaused() const;
```
Checks whether the capture is paused.
- **returns:** ``true`` if the capture is paused, otherwise ``false``.
<br><br><br><br>

```c++
uint32_t GetDeviceEnumerationPeriod() const;
```
Gets the time waited, in milliseconds, between each device enumeration.
- **returns:** The time waited between each device enumeration in milliseconds.
<br><br><br><br>

```c++
void SetDeviceEnumerationPeriod(uint32_t deviceEnumerationPeriod_ms);
```
Sets the time waited, in milliseconds, between each device enumeration.
- **deviceEnumerationPeriod_ms:** The time waited between each device enumeration in milliseconds.
<br><br><br><br>

```c++
void SetMasterVolume(heph_float volume);
```
Sets the master volume.
- **volume:** The master volume.

> [!CAUTION]
> Values above 1 may cause distortion.

<br><br><br><br>

```c++
heph_float GetMasterVolume() const;
```
Gets the master volume.
- **returns:** The master volume.
<br><br><br><br>

```c++
AudioFormatInfo GetRenderFormat() const;
```
Gets the render format.
- **returns:** The render format info.
<br><br><br><br>

```c++
AudioFormatInfo GetCaptureFormat() const;
```
Gets the capture format.
- **returns:** The capture format info.
<br><br><br><br>

```c++
void InitializeRender();
```
Initializes render with the default device and the default format.
<br><br><br><br>

```c++
void InitializeRender(uint16_t channelCount,
                      uint32_t sampleRate);
```
Initializes render with the default device and the internal format.
- **channelCount:** Number of channels. Closest channel count will be selected if the provided one is not supported.
- **sampleRate:** Sample rate. Closest sample rate will be selected if the provided one is not supported.
<br><br><br><br>

```c++
void InitializeRender(AudioFormatInfo format);
```
Initializes render with the default device and the provided format.
- **format:** The render format.
<br><br><br><br>

```c++
void InitializeRender(AudioDevice* device,
                      AudioFormatInfo format);
```
Initializes render with the provided device and the provided format.
- **device:** Pointer to the render device which will be used. Can be ``nullptr`` if the default device is desired.
- **format:** The render format. Closest format will be selected if the provided one is not supported.
<br><br><br><br>

```c++
void StopRendering();
```
Stops rendering and frees the resources used for rendering.
<br><br><br><br>

```c++
void InitializeCapture();
```
Initializes capture with the default device and the default format.
<br><br><br><br>

```c++
void InitializeCapture(uint16_t channelCount,
                       uint32_t sampleRate);
```
Initializes capture with the default device and the internal format.
- **channelCount:** Number of channels. Closest channel count will be selected if the provided one is not supported.
- **sampleRate:** Sample rate. Closest sample rate will be selected if the provided one is not supported.
<br><br><br><br>

```c++
void InitializeCapture(AudioFormatInfo format);
```
Initializes capture with the default device and the provided format.
- **format:** The capture format.
<br><br><br><br>

```c++
void InitializeCapture(AudioDevice* device,
                       AudioFormatInfo format);
```
Initializes capture with the provided device and the provided format.
- **device:** Pointer to the capture device which will be used. Can be ``nullptr`` if the default device is desired.
- **format:** The capture format. Closest format will be selected if the provided one is not supported.
<br><br><br><br>

```c++
void StopCapturing();
```
Stops capturing and frees the resources used for capturing.
<br><br><br><br>

```c++
AudioDevice GetAudioDeviceById(const std::string& deviceId) const;
```
Gets the [AudioDevice](/docs/HephAudio/AudioDevice.md) with the provided id.
- **deviceId:** Unique id of the audio device.
- **returns:** The audio device if found, otherwise a default [AudioDevice](/docs/HephAudio/AudioDevice.md) instance.
<br><br><br><br>

```c++
AudioDevice GetRenderDevice() const;
```
Gets the [AudioDevice](/docs/HephAudio/AudioDevice.md) that's currently used for rendering.
- **returns:** The audio device that's currently used for rendering.
<br><br><br><br>

```c++
AudioDevice GetCaptureDevice() const;
```
Gets the [AudioDevice](/docs/HephAudio/AudioDevice.md) that's currently used for capturing.
- **returns:** The audio device that's currently used for capturing.
<br><br><br><br>

```c++
AudioDevice GetDefaultAudioDevice(AudioDeviceType deviceType) const;
```
Gets the [AudioDevice](/docs/HephAudio/AudioDevice.md) that's the system default for rendering/capturing. 
- **deviceType:** Type of the default audio device. This can be either ``AudioDeviceType::Render`` or ``AudioDeviceType::Capture``.
- **returns:** The default audio device used by the system.

> [!IMPORTANT]
> This is not supported by all of the native APIs.

<br><br><br><br>

```c++
std::vector<AudioDevice> GetAudioDevices(AudioDeviceType deviceType) const;
```
Gets all the available [AudioDevices](/docs/HephAudio/AudioDevice.md) of the requested type.
- **deviceType:** Type of the audio devices. This can be ``AudioDeviceType::Render``, ``AudioDeviceType::Capture`` or ``AudioDeviceType::All``.
- **returns:** All available audio devices of the requested type.
<br><br><br><br>

```c++
bool SaveToFile(AudioBuffer& buffer,
                const std::string& filePath,
                bool overwrite);
```
Creates a file and saves the provided data.
- **buffer:** Audio data which will be saved.
- **filePath:** Path of the file.
- **overwrite:** Indicates whether to write over the file if it already exists.
- **returns:** ``true`` if the operation has succeeded, otherwise ``false``.
<br><br><br><br>

```c++
void SetOnAudioDeviceAddedHandler(HephCommon::EventHandler handler);
```
Sets the provided handler as the only event handler.
- **handler:** The method to handle the event.
<br><br><br><br>

```c++
void AddOnAudioDeviceAddedHandler(HephCommon::EventHandler handler);
```
Adds the provided handler.
- **handler:** The method to handle the event.
<br><br><br><br>

```c++
void SetOnAudioDeviceRemovedHandler(HephCommon::EventHandler handler);
```
Sets the provided handler as the only event handler.
- **handler:** The method to handle the event.
<br><br><br><br>

```c++
void AddOnAudioDeviceRemovedHandler(HephCommon::EventHandler handler);
```
Adds the provided handler.
- **handler:** The method to handle the event.
<br><br><br><br>


```c++
void SetOnCaptureHandler(HephCommon::EventHandler handler);
```
Sets the provided handler as the only event handler.
- **handler:** The method to handle the event.
<br><br><br><br>

```c++
void AddOnCaptureHandler(HephCommon::EventHandler handler);
```
Adds the provided handler.
- **handler:** The method to handle the event.
