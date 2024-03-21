## AudioPlaylist class
```c++
#include <AudioPlaylist.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Enums](#enums)<br>
[Methods](#methods)


### Description
Class for creating playlists. Uses [AudioStream](/docs/HephAudio/AudioStream.md) internally to play the files.
<br><br>


### Macros

```c++
#define HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY
```
The key to find the ``AudioPlaylist`` instance when handling events.
<br><br>


### Enums
```c++
enum TransitionEffect : uint8_t
{
    None,
    Delay,
    Fade,
    FadeIn,
    FadeOut
};
```
Indicates which effect will be applied while switching from one file to another.


### Methods

```c++
AudioPlaylist(Native::NativeAudio* pNativeAudio);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the ``NativeAudio`` instance that will be used to play the files.
<br><br><br><br>

```c++
AudioPlaylist(Audio& audio);
```
Creates an instance and initializes it with the provided values.
- **audio:** Reference of the ``Audio`` instance that will be used to play the files.
<br><br><br><br>

```c++
AudioPlaylist(Native::NativeAudio* pNativeAudio,
              const std::vector<HephCommon::StringBuffer>& files);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the ``NativeAudio`` instance that will be used to play the files.
- **files:** File paths, stored in the order they will be played.
<br><br><br><br>

```c++
AudioPlaylist(Audio& audio,
              const std::vector<HephCommon::StringBuffer>& files);
```
Creates an instance and initializes it with the provided values.
- **audio:** Reference of the ``Audio`` instance that will be used to play the files.
- **files:** File paths, stored in the order they will be played.
<br><br><br><br>

```c++
AudioPlaylist(Native::NativeAudio* pNativeAudio,
              TransitionEffect transitionEffect,
              heph_float transitionDuration_s);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the ``NativeAudio`` instance that will be used to play the files.
- **transitionEffect:** The effect that will be applied while switching from one file to another.
- **transitionDuration_s:** Duration of the transition effect in seconds.
<br><br><br><br>

```c++
AudioPlaylist(Audio& audio,
              TransitionEffect transitionEffect,
              heph_float transitionDuration_s);
```
Creates an instance and initializes it with the provided values.
- **audio:** Reference of the ``Audio`` instance that will be used to play the files.
- **transitionEffect:** The effect that will be applied while switching from one file to another.
- **transitionDuration_s:** Duration of the transition effect in seconds.
<br><br><br><br>

```c++
AudioPlaylist(Native::NativeAudio* pNativeAudio,
              TransitionEffect transitionEffect,
              heph_float transitionDuration_s,
              const std::vector<HephCommon::StringBuffer>& files);
```
Creates an instance and initializes it with the provided values.
- **pNativeAudio:** Pointer to the ``NativeAudio`` instance that will be used to play the files.
- **transitionEffect:** The effect that will be applied while switching from one file to another.
- **transitionDuration_s:** Duration of the transition effect in seconds.
- **files:** File paths, stored in the order they will be played.
<br><br><br><br>

```c++
AudioPlaylist(Audio& audio,
              TransitionEffect transitionEffect,
              heph_float transitionDuration_s,
              const std::vector<HephCommon::StringBuffer>& files);
```
Creates an instance and initializes it with the provided values.
- **audio:** Reference of the ``Audio`` instance that will be used to play the files.
- **transitionEffect:** The effect that will be applied while switching from one file to another.
- **transitionDuration_s:** Duration of the transition effect in seconds.
- **files:** File paths, stored in the order they will be played.
<br><br><br><br>

```c++
AudioPlaylist(AudioPlaylist&& rhs) noexcept;
```
Creates an instance and moves the ownership of the contents of ``rhs`` to it.
- **rhs:** The instance whose contents will be moved.
<br><br><br><br>

```c++
AudioPlaylist& operator=(AudioPlaylist&& rhs) noexcept;
```
Releases the resources of the current playlist, then moves the ownership of the contents of ``rhs`` to it.
- **rhs:** The instance whose contents will be moved.
- **returns:** Reference to the current playlist.
<br><br><br><br>

```c++
size_t Size() const;
```
Gets the number of files present in the playlist.
- **returns:** Number of files in the playlist.
<br><br><br><br>

```c++
Native::NativeAudio* GetNativeAudio() const;
```
Gets the pointer to the ``NativeAudio`` instance that's used for playing the files.
- **returns:** Pointer to the ``NativeAudio`` instance.
<br><br><br><br>

```c++
TransitionEffect GetTransitionEffect() const;
```
Gets the transition effect.
- **returns:** The transition effect.
<br><br><br><br>

```c++
void SetTransitionEffect(TransitionEffect transitionEffect);
```
Sets the transition effect.
- **transitionEffect:** The transition effect.
<br><br><br><br>

```c++
heph_float GetTransitionDuration() const;
```
Gets the transition duration in seconds.
- **returns:** The transition duration in seconds.
<br><br><br><br>

```c++
void SetTransitionDuration(heph_float transitionDuration_s);
```
Sets the transition duration in seconds.
- **transitionDuration_s:** The transition duration in seconds.
<br><br><br><br>

```c++
void Start();
```
Starts playing the files.
<br><br><br><br>

```c++
void Stop();
```
Stops playing the files.
<br><br><br><br>

```c++
bool IsPaused() const;
```
Gets whether the playlist is paused (stopped).
- **returns:** ``true`` if the playlist is paused, otherwise ``false``.
<br><br><br><br>

```c++
void Add(const HephCommon::StringBuffer& filePath);
```
Adds a file to the end of the playlist.
- **filePath:** File path.
<br><br><br><br>

```c++
void Add(const std::vector<HephCommon::StringBuffer>& files);
```
Adds the provided files to the end of the playlist.
- **files:** File paths.
<br><br><br><br>

```c++
void Insert(const HephCommon::StringBuffer& filePath,
            size_t index);
```
Adds the provided file to the playlist.
- **filePath:** File path.
- **index:** Position of the new file within the playlist.
<br><br><br><br>

```c++
void Insert(const std::vector<HephCommon::StringBuffer>& files,
            size_t index);
```
Adds the provided files to the playlist.
- **files:** File paths.
- **index:** Position of the new files within the playlist.
<br><br><br><br>

```c++
void Remove(size_t index);
```
Removes the file at the provided index from the playlist.
- **index:** Position of the file that will be removed within the playlist.
<br><br><br><br>

```c++
void Remove(size_t index,
            size_t count);
```
Removes the files at the provided index from the playlist.
- **index:** Position of the files that will be removed within the playlist.
- **count:** Number of files that will be removed.
<br><br><br><br>

```c++
void Remove(const HephCommon::StringBuffer& filePath);
```
Removes the file with the provided path from the playlist.
- **filePath:** Path of the file that will be removed.
<br><br><br><br>

```c++
void Skip();
```
Skips to the next file.
<br><br><br><br>

```c++
void Skip(size_t n);
```
Skips the first ``n`` files.
<br><br><br><br>

```c++
void Clear();
```
Removes all of the files.
<br><br><br><br>
