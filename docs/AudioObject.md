## AudioObject Struct

> [Description](#description)<br>
[Macros](#macros)<br>
[Parameters](#parameters)<br>
[Methods](#methods)

### Description
Holds the information that's necessary to play audio.
<br><br>

### Macros
```c++
#define HEPHAUDIO_INFINITE_LOOP 0
```

```c++
#define HEPHAUDIO_RENDER_HANDLER_DEFAULT
```
The default handler for the ``AudioObject::OnRender`` event. Plays the audio data as is.
<br><br>

```c++
#define HEPHAUDIO_RENDER_HANDLER_ENSURE_FORMAT
```
An event handler for the ``AudioObject::OnRender`` event. Converts the audio data to the render format before playing.
<br><br><br><br>

### Parameters
```c++
struct AudioObject
{
	HephCommon::Guid id;
	HephCommon::StringBuffer filePath;
	HephCommon::StringBuffer name;
	bool isPaused;
	uint32_t playCount;
	heph_float volume;
	AudioBuffer buffer;
	size_t frameIndex;
	HephCommon::Event OnRender;
	HephCommon::Event OnFinishedPlaying;
};
```

- ``id``
<br><br>
Unique identifier.
<br><br>

- ``filePath``
<br><br>
Path of the file the object created with, or empty if created via ``NativeAudio::CreateAudioObject``.
<br><br>

- ``name``
<br><br>
Name of the object. Objects can share the same name.
<br><br>

- ``isPaused``
<br><br>
Indicates whether the object is paused. If true, the object will not be played until this field is set to false.
<br><br>

- ``playCount``
<br><br>
Number of times the object will be played. Set this to ``HEPHAUDIO_INFINITE_LOOP`` in order to play it infinite times.
<br><br>

- ``volume``
<br><br>
Loudness of the audio between 0 and 1.
> [!CAUTION]
> Values above 1 may cause distortion.

<br>

- ``buffer``
<br><br>
The audio buffer that contains the audio data.
<br><br>

- ``frameIndex``
<br><br>
Index of the first audio frame that will be rendered (played) next. 
<br><br>

- ``OnRender``
<br><br>
Event that will be invoked each time before rendering (playing) audio data. 
<br><br>

- ``OnFinishedPlaying``
<br><br>
Event that will be invoked each time when the object finishes playing. 
<br><br>

### Methods
```c++
AudioObject();
```
Creates an instance and initializes it with default values.
<br><br><br><br>

```c++
AudioObject(AudioObject&& rhs) noexcept;
```
Creates an instance and initializes it by moving the rhs's fields.
- **rhs:** The object to move.
<br><br><br><br>

```c++
AudioObject& operator=(AudioObject&& rhs) noexcept;
```
Moves the rhs's fields to the current object.
- **rhs:** The object to move.
- **Returns:** The current object.
<br><br><br><br>

```c++
heph_float GetPosition() const;
```
Calculates the position of the first audio frame which will be rendered next. Returns it between 0 and 1.
- **Returns:** Position of the frame that will be rendered next.
<br><br><br><br>

```c++
void SetPosition(heph_float position);
```
Sets the frameIndex by the provided position.
- **Position:** Position of the frame that will be rendered next. Must be between 0 and 1.
<br><br><br><br>

```c++
static void DefaultRenderHandler(const HephCommon::EventParams& eventParams);
```
The default handler for the ``AudioObject::OnRender`` event. Plays the audio data as is.
- **eventParams:** Event parameters.
<br><br><br><br>

```c++
static void EnsureFormatRenderHandler(const HephCommon::EventParams& eventParams);
```
An event handler for the ``AudioObject::OnRender`` event. Converts the audio data to the render format before playing.
- **eventParams:** Event parameters.
<br><br><br><br>
