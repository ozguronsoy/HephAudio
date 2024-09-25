# HephAudio

[![Static Badge](https://img.shields.io/badge/License-LGPL%20v2.1-blue)](https://github.com/ozguronsoy/HephAudio/blob/master/LICENSE.md)
[![GitHub Pages](https://github.com/ozguronsoy/HephAudio/actions/workflows/github_pages.yml/badge.svg?branch=master)](https://github.com/ozguronsoy/HephAudio/actions/workflows/github_pages.yml)
[![Build Linux](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_linux.yml/badge.svg?branch=master&event=push)](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_linux.yml)
[![Build Windows](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_windows.yml/badge.svg?branch=master&event=push)](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_windows.yml)
[![Build Android](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_android.yml/badge.svg?branch=master&event=push)](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_android.yml)

- [Setup](#setup)<br>
	- [CMake](#cmake)<br>
	- [Visual Studio](#visual-studio)<br>
- [Getting Started](#getting-started)<br>
	- [Playing Files](#playing-files)<br>
	- [Recording](#recording)<br>
	- [Device Enumeration](#device-enumeration)<br>
	- [Applying Effects](#applying-effects)<br>
	- [Handling Exceptions](#handling-exceptions)<br>
- [Documentation](https://ozguronsoy.github.io/HephAudio/)<br>
- [Examples](https://github.com/ozguronsoy/HephAudio/tree/master/docs/examples)<br>

# Introduction
HephAudio is a cross-platform audio library that provides:<br>
- Playing and recording audio in Windows, Linux, Android, iOS, and macOS.<br>
- Audio device enumeration and selection.<br>
- Encoding, decoding, and transcoding audio files via [FFmpeg](https://ffmpeg.org/).<br>
- FFT for frequency analysis of the audio signals.<br>
- Spatialization via HRTF.<br>
- Easy to use sound effects and filters.<br>

# Setup

## CMake

### Create Shared/Static Library
1) Clone the repo.<br>
2) Run one of the following commands:
	- to create shared library: ``cmake -DENABLE_SHARED=On -DCMAKE_CXX_FLAGS='-DHEPHAUDIO_INFO_LOGGING' .``<br>
	- to create static library: ``cmake -DENABLE_STATIC=On -DCMAKE_CXX_FLAGS='-DHEPHAUDIO_INFO_LOGGING' .``<br>
3) Run ``cmake --build .`` then ``cmake --install .``<br>
4) Create a folder at your project's root and name it ``HephAudio`` (/project_root/HephAudio).<br>
5) Copy the contents of the ``build`` folder to ``/project_root/HephAudio``.<br>
6) Create a ``CMakeLists.txt`` file at your project's root folder and build it.<br>

An example cmake file:
```
cmake_minimum_required(VERSION 3.25)

# your project name
project("my_application")

set(HEPHAUDIO_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/HephAudio)

if ((NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY) OR (CMAKE_RUNTIME_OUTPUT_DIRECTORY STREQUAL ""))
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
endif()

add_definitions(-DHEPH_SHARED_LIB)

include_directories(
    ${HEPHAUDIO_DIRECTORY}/include/ffmpeg/
    ${HEPHAUDIO_DIRECTORY}/include/libmysofa/
    ${HEPHAUDIO_DIRECTORY}/include/HephCommon/
    ${HEPHAUDIO_DIRECTORY}/include/HephAudio/
)

add_executable(
    ${CMAKE_PROJECT_NAME}
    main.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")

    target_link_libraries(
        ${CMAKE_PROJECT_NAME}

        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avcodec.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avdevice.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avfilter.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avformat.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avutil.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/swresample.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/swscale.lib

        ${HEPHAUDIO_DIRECTORY}/lib/libmysofa/zlib.lib
        ${HEPHAUDIO_DIRECTORY}/lib/libmysofa/mysofa.lib

        ${HEPHAUDIO_DIRECTORY}/lib/HephAudio.lib
    )

    # copy the DLL files so they will be in the same folder with the executable.
    install(
        DIRECTORY ${HEPHAUDIO_DIRECTORY}/
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        FILES_MATCHING
        PATTERN "*.dll"
        PATTERN "include" EXCLUDE
        PATTERN "lib" EXCLUDE
    )

else ()

    target_link_libraries(
        ${CMAKE_PROJECT_NAME}

        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavcodec.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavdevice.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavfilter.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavformat.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavutil.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libswresample.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libswscale.so

        ${HEPHAUDIO_DIRECTORY}/lib/libmysofa/libmysofa.so

        ${HEPHAUDIO_DIRECTORY}/lib/libHephAudio.so
    )

endif()
```


### Use Directly
1) Create a folder at your project's root and name it ``HephAudio`` (/project_root/HephAudio).<br>
2) Copy the repo to the folder you created.<br>
3) **(WINDOWS ONLY)** Copy the required dll files from the dependencies to the build output folder.
4) Create a ``CMakeLists.txt`` file at your project's root folder and build it.<br>

An example cmake file:

```
cmake_minimum_required(VERSION 3.25)

# your project name
project("my_application")

# execute the HephAudio/CMakeLists.txt file
include(${CMAKE_CURRENT_SOURCE_DIR}/HephAudio/CMakeLists.txt)

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${HEPHAUDIO_SRC}
    # your files
    main.cpp
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    ${HEPHAUDIO_LINK_LIBS}
    # your libs
)

# extra definitions
add_definitions(-DHEPHAUDIO_INFO_LOGGING)
```

<br><br>

## Visual Studio
1) Create a folder at your project's root and name it ``HephAudio`` (/project_root/HephAudio).<br>
2) Copy the repo to the folder you created.<br>
3) Right click to your project, go to ``Configuration Properties -> C/C++ -> General -> Additional Including Directories`` and add the locations of the ``HephCommon/HeaderFiles``, ``HephAudio/HeaderFiles``, ``dependencies/ffmpeg/include``, and ``dependencies/libmysofa/include``.<br>
4) Now right click the solution and go to ``Add -> Existing Project``, under the HephCommon folder select ``HephCommon.vcxitems`` to add to your project. Repeat the same process for HephAudio.<br>
5) Right click to your project, ``Add -> Reference -> Shared Projects`` and check both HephAudio and HephCommon.<br>
6) Right click to your project, go to ``Configuration Properties -> Linker -> General -> Additional Library Directories`` and add ``path_to_hephaudio/dependencies``.<br>
7) Copy the required dll files from the dependencies to the build output folder.<br>
8) Visual studio marks some of the standard C functions as unsafe and prevents from compiling by throwing errors. To fix this, right click to your project and go to ``Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions`` and add ``_CRT_SECURE_NO_WARNINGS``.<br>
9) If you are creating a DLL, add ``HEPH_EXPORTS`` and ``HEPH_SHARED_LIB`` preprocessor definitions.

> [!NOTE]
> Don't define ``HEPH_EXPORTS`` when using the DLL.

# Getting Started

### Playing Files

Let's start by playing a wav file with the default output device. First we need to initialize the audio class for rendering (playing) by calling the ``InitializeRender`` method. Then simply call the ``Play`` method with the full path to the wav file.

```cpp
#include <iostream>
#include <string.h>
#include <Audio.h>

using namespace HephAudio;

int main()
{
	Audio audio;
	
	// start rendering
	audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

	// load the audio data into memory and start playing
	audio.Play("some_path\\some_file.wav");

	// prevent from exiting the app
	std::string s;
	std::cin >> s;
  
	return 0;
}
```

If the audio data we want to play does not have the same format as the one we specified when initializing render, the sound will come out distorted. To prevent that we can call the ``Load`` method instead. This method converts the audio data to our target format before playing.

```cpp
audio.Load("some_path\\some_file.wav", false); // false = don't pause.
```

We can also do these convertions on the samples that are just about to be played so we don't have to wait for converting all the data before start playing.

### Recording

To record audio, first we need to initialize capturing just like before, this will start the recording process. You can access the recorded data via ``OnCapture`` event, which is invoked when some amount of data is captured (typically 10ms of audio data).
To add an event handler use either one of the following methods:

```cpp
audio.SetOnCaptureHandler(&MyCallbackMethod); // Removes all the other event handlers, then adds the provided one
audio.AddOnCaptureHandler(&MyCallbackMethod); // Adds the provided event handler
```

After setting an event handler for the ``OnCapture`` event, we must cast the provided ``EventArgs`` pointer to the correct type to access the captured data and append it to an ``AudioBuffer`` we created. For more details about the ``EventArgs`` and ``EventResult`` visit the documentation.
When you are done recording you can either call ``StopCapturing()`` method to deinitialize capturing or ``PauseCapture`` method to stop raising the ``OnCapture`` event.

Sample code for recording audio for 5 seconds, saving it to a file, then playing the file:

```cpp
#include <iostream>
#include <string.h>
#include <thread>
#include <chrono>
#include <filesystem>
#include <Audio.h>
#include <AudioProcessor.h>

#define RECORD_AUDIO_FILE_PATH "some_path\\some_file.wav"

using namespace Heph;
using namespace HephAudio;

// will store the audio we recorded.
AudioBuffer recordedAudio;

// the event handler method
void RecordAudio(const EventParams& eventParams)
{
	AudioCaptureEventArgs* pCaptureArgs = (AudioCaptureEventArgs*)eventParams.pArgs;
	recordedAudio.Append(pCaptureArgs->captureBuffer);
}

int main()
{
	Audio audio;

	audio.InitializeCapture();
	recordedAudio = AudioBuffer(0, audio.GetCaptureFormat().channelLayout, audio.GetCaptureFormat().sampleRate);

	// set an event handler for capturing
	audio.SetOnCaptureHandler(&RecordAudio);

	// record for 5 seconds, then stop capturing
	std::this_thread::sleep_for(std::chrono::seconds(5));
	audio.StopCapturing();

	// Save the recorded audio data to a file.
	std::shared_ptr<IAudioEncoder> pEncoder = audio.GetAudioEncoder();
	pEncoder->ChangeFile(RECORD_AUDIO_FILE_PATH, recordedAudio.FormatInfo(), true);
	pEncoder->Encode(recordedAudio);
	pEncoder->CloseFile();

	recordedAudio.Release(); // dispose of the unnecessary data

	// play the file.
	audio.InitializeRender();
	audio.Play(RECORD_AUDIO_FILE_PATH);

	// prevent from exiting the app
	std::string s;
	std::cin >> s;

	return 0;
}
```

### Device Enumeration

Available audio devices are updated periodically.

```cpp
// update every 3 seconds
audio.SetDeviceEnumerationPeriod(3000);
```

To get a list of available audio devices call the ``GetAudioDevices`` method.

```cpp
std::vector<AudioDevice> renderDevices = audio.GetAudioDevices(AudioDeviceType::Render);
std::vector<AudioDevice> captureDevices = audio.GetAudioDevices(AudioDeviceType::Capture);
std::vector<AudioDevice> allDevices = audio.GetAudioDevices(AudioDeviceType::All);
```

To get the default audio device, call the ``GetDefaultAudioDevice`` method.

```cpp
AudioDevice defaultRenderDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Render);
AudioDevice defaultCaptureDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Capture);
```

After obtaining the audio devices, simply pass a pointer of the desired audio device to the ``InitializeRender`` method.

```cpp
audio.InitializeRender(&renderDevices[0], AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000));
```

### Applying Effects

Most of the signal processing is done by the ``AudioProcessor`` class. To apply effects we must obtain the ``AudioBuffer`` that's storing the audio data. We can obtain this and any other data that's necessarry to play audio from the ``AudioObject`` that is returned by the ``Play`` and ``Load`` methods.
Here is a sample code for playing the file in 2x speed without changing the pitch:

```cpp
#include <iostream>
#include <string.h>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Windows/HannWindow.h>

using namespace HephAudio;

int main()
{
	Audio audio;
	audio.InitializeRender();

	AudioObject* pAudioObject = audio.Load("some_path\\some_file.wav");

	std::cout << "applying sound effects..." << std::endl;

	// plays in 2x speed without changing the pitch
	HannWindow window;
	AudioProcessor::ChangeSpeed(pAudioObject->buffer, 2.0, window);

	std::cout << "sound effects applied!" << std::endl;

	// unpause
	pAudioObject->isPaused = false;

	// prevent from exiting the app
	std::string s;
	std::cin >> s;

	return 0;
}
```

### Handling Exceptions

Every error that occurs while using the library raises an ``OnException`` event but only some exception will actually throw. Using this event we can log the exception details to a file or for this instance, simply print it to console.

In this example, we will try to open a non-existing file. This is going to throw an exception.

```cpp
#include <Exceptions/Exception.h>
#include <Exceptions/InvalidArgumentException.h>
#include <Exceptions/InsufficientMemoryException.h>

using namespace Heph;

int main()
{
	Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

	HEPH_RAISE_EXCEPTION(nullptr, InvalidArgumentException(HEPH_FUNC, "invalid argument!"));
	
	HEPH_RAISE_AND_THROW_EXCEPTION(nullptr, InsufficientMemoryException(HEPH_FUNC, "insufficient memory!"));
	
	return 0;
}
```

With this we conclude our introduction to the HephAudio library. To learn more check [examples](https://github.com/ozguronsoy/HephAudio/tree/master/docs/examples) or the [documentation](https://ozguronsoy.github.io/HephAudio/).
