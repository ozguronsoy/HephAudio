## AudioStream class
```c++
#include <AudioStream.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Methods](#methods)


### Description
Class for playing audio files without loading them into memory. 
Reads the portion of audio data from the file just before rendering.
<br><br>

### Macros

```c++
#define HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY
```
The key to find the ``AudioStream`` instance when handling events.
<br><br>

### Methods
```c++
AudioStream(Native::NativeAudio* pNativeAudio);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the ``NativeAudio`` instance that will be used to render audio data.
<br><br><br><br>

```c++
AudioStream(Audio& audio);
```
Creates an instance and initializes it with the provided values.
- **audio:** Reference of the ``Audio`` instance that will be used to render audio data.
<br><br><br><br>

```c++
AudioStream(Native::NativeAudio* pNativeAudio,
            const std::string& filePath);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the ``NativeAudio`` instance that will be used to render audio data.
- **filePath:** Path of the file that will be played.
<br><br><br><br>

```c++
AudioStream(Audio& audio,
            const std::string& filePath);
```
Creates an instance and initializes it with the provided values.
- **audio:** Reference of the ``Audio`` instance that will be used to render audio data.
- **filePath:** Path of the file that will be played.
<br><br><br><br>

```c++
AudioStream(AudioStream&& rhs) noexcept;
```
Creates an instance and moves the ownership of the contents of ``rhs`` to it.
- **rhs:** The stream instance whose contents will be moved.
<br><br><br><br>

```c++
~AudioStream();
```
Releases the resources.
<br><br><br><br>

```c++
AudioStream& operator=(AudioStream&& rhs) noexcept;
```
Releases the resources of the current stream, then moves the ownership of the contents of ``rhs`` to it.
- **rhs:** The stream instance whose contents will be moved.
- **returns:** Reference to the current stream.
<br><br><br><br>

```c++
Native::NativeAudio* GetNativeAudio() const;
```
Gets the pointer to the ``NativeAudio`` instance that's used for rendering audio data.
- **returns:** Pointer to the ``NativeAudio`` instance.
<br><br><br><br>

```c++
AudioObject* GetAudioObject() const;
```
Gets the ``AudioObject`` instance that's created to play audio data.
- **returns:** The ``AudioObject`` instance.
<br><br><br><br>

```c++
const AudioFormatInfo& GetAudioFormatInfo() const;
```
Gets the format info of the audio data.
- **returns:** The format info.
<br><br><br><br>

```c++
size_t GetFrameCount() const;
```
Gets the number of audio frames the file contains.
- **returns:** Number of audio frames the file contains.
<br><br><br><br>

```c++
void ChangeFile(const std::string& newFilePath);
```
Changes the file that's currently playing with the provided one.
- **newFilePath:** Path of the file that will be played.
<br><br><br><br>

```c++
void Start();
```
Starts (resumes) playing the file.
<br><br><br><br>

```c++
void Stop();
```
Stops (pauses) playing the file.
<br><br><br><br>

```c++
heph_float GetPosition() const;
```
Gets the position of the frame that will be rendered next between 0 and 1.
- **returns:** Position between 0 and 1.
<br><br><br><br>

```c++
void SetPosition(heph_float position);
```
Sets the position of the frame that will be rendered next between 0 and 1.
- **position:** Position between 0 and 1.
<br><br><br><br>

```c++
void Release();
```
Releases the resources.
<br><br><br><br>
