# Introduction
HephAudio is a cross-platform audio library that provides:
- Playing and recording audio data in Windows, Linux, iOS, macOS, and Android.
- Audio device enumeration and selection.
- Tools for storing and processing audio data with ease.
- Easy to use sound effects and filters.
- Spatialization via HRTF.
- FFT for frequency analysis of the audio signals.
- Encoding, decoding, and transcoding audio files via [FFmpeg](https://ffmpeg.org/).

# Setup
### Visual Studio
1) Create a folder at your project's root and name it ``HephAudio`` (/project_root/HephAudio).
2) Copy the repo to the folder you created.
3) Right click to your project, go to ``Configuration Properties -> C/C++ -> General -> Additional Including Directories`` and add the locations of the ``HephCommon/HeaderFiles``, ``HephAudio/HeaderFiles``, and  ``dependencies/ffmpeg/include``.
4) Now right click the solution and go to ``Add -> Existing Project``, under the HephCommon folder select ``HephCommon.vcxitems`` to add to your project. Repeat the same process for HephAudio.
5) Right click to your project, ``Add -> Reference -> Shared Projects`` and check both HephAudio and HephCommon.
6) Right click to your project, go to ``Configuration Properties -> Linker -> General -> Additional Library Directories`` and add ``path_to_hephaudio/dependencies``.
7) Copy the required dll files from the dependencies to the build output folder.
8) Visual studio marks some of the standard functions as unsafe and prevents from compiling by throwing errors. To fix this, right click to your project and go to ``Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions`` and add ``_CRT_SECURE_NO_WARNINGS``.
<br><br>

### VS Code
1) Create a folder at your project's root and name it ``HephAudio`` (/project_root/HephAudio).
2) Copy the repo to the folder you created.
3) **(WINDOWS ONLY)** Copy the required dll files from the dependencies to the build output folder.
4) Create a ``CMakeLists.txt`` file at your project's root folder and build it.<br>
An example cmake file:
```
cmake_minimum_required(VERSION 3.22.1)

# your project name
project("myapplication")

# if not set, HephAudio/CMakeLists.txt will set the CMAKE_CXX_STANDARD to 17
set (CMAKE_CXX_STANDARD 17)

# execute the HephAudio/CMakeLists.txt file
include(${CMAKE_CURRENT_SOURCE_DIR}/HephAudio/CMakeLists.txt)

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${HEPH_AUDIO}
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

# Getting Started
### Playing Files
Let's start by playing a wav file with the default output device. First we need to initialize the audio class for rendering (playing) by calling the ``InitializeRender`` method. Then simply call the ``Play`` method with the full path to the wav file.
```cpp
#include <iostream>
#include <string.h>
#include <Audio.h>

using namespace HephAudio; // so we don't have to write HephAudio:: every time

int main()
{
	Audio audio;

	// 48000 -> 48 kHz sampling rate.
	audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

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
We can also do these convertions on the samples that are just about to be played so we don't have to wait for converting all the data before start playing. You can find more information on this in the documentation files.

### Recording
To record audio, first we need to initialize capturing just like before, this will start the recording process. You can access the recorded data via ``OnCapture`` event, which is invoked when some amount of data is captured (typically 10ms of audio data).
To add an event handler use either one of the following methods:
```cpp
audio.SetOnCaptureHandler(&MyCallbackMethod); // Removes all the other event handlers, then adds the provided one
audio.AddOnCaptureHandler(&MyCallbackMethod); // Adds the provided event handler
```

After setting an event handler for the ``OnCapture`` event, we must cast the provided ``EventArgs`` pointer to the correct type to access the captured data and append it to an ``AudioBuffer`` we created. For more details about the ``EventArgs`` and ``EventResult`` visit the documentation.
When you are done recording you can either call ``StopCapturing()`` method to deinitialize capturing or ``PauseCapture(bool isPaused)`` method to prevent from raising the ``OnCapture`` event.

Sample code for recording audio for 5 seconds, saving it to a file, then playing the file:
```cpp
#include <iostream>
#include <string.h>
#include <thread>
#include <chrono>
#include <filesystem>
#include <Audio.h>
#include <AudioProcessor.h>

using namespace Heph;
using namespace HephAudio;

AudioBuffer recordedAudio;

void RecordAudio(const EventParams& eventParams) // the event handler method
{
	AudioCaptureEventArgs* pCaptureArgs = (AudioCaptureEventArgs*)eventParams.pArgs; // cast the args to capture event args
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
	audio.GetAudioEncoder()->ChangeFile("some_path/some_file.wav", recordedAudio.FormatInfo(), true);

	audio.GetAudioEncoder()->Encode(recordedAudio);

	recordedAudio.Release(); // dispose of the unnecessary data

	// play the recorded file.
	audio.InitializeRender();
	audio.Play("some_path/some_file.wav");

	// prevent from exiting the app
	std::string s;
	std::cin >> s;

	return 0;
}
```

### Device Enumeration
To get a list of available audio devices call the ``GetAudioDevices`` method.
```cpp
std::vector<AudioDevice> renderDevices = audio.GetAudioDevices(AudioDeviceType::Render);    // Get only the devices that are capable of rendering
std::vector<AudioDevice> captureDevices = audio.GetAudioDevices(AudioDeviceType::Capture);  // Get only the devices that are capable of capturing
std::vector<AudioDevice> audioDevices = audio.GetAudioDevices(AudioDeviceType::All);        // Get all devices.
```
To get the default audio device, call the ``GetDefaultAudioDevice`` method.
```cpp
AudioDevice defaultRenderDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Render);
AudioDevice defaultCaptureDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Capture);
```
After obtaining the audio devices, simply pass a pointer of the desired audio device to the ``InitializeRender`` method.
```cpp
audio.InitializeRender(&renderDevices[0], AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, 2, 16, 48000));
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
	audio.InitializeRender(nullptr, AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000));

	AudioObject* pAudioObject = audio.Load("some_path\\some_file.wav", true); // pause before applying effects.

	printf("applying sound effects...\n");

	HannWindow window; // windows will be explained later in the docs, for now select a HannWindow
	AudioProcessor::ChangeSpeed(pAudioObject->buffer, 2.0, window); // plays in 2x speed without changing the pitch

	printf("sound effects applied!\n");

	pAudioObject->isPaused = false; // unpause

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
#include <HephException.h>
#include <ConsoleLogger.h>
#include <StringHelpers.h>

using namespace HephCommon;

void HandleExceptions(const EventParams& eventParams)
{
	const HephException& ex = ((HephExceptionEventArgs*)eventParams.pArgs)->exception; // get the exception data

	std::string exceptionString = "Error!\n" + ex.method + " (" + StringHelpers::ToHexString(ex.errorCode) + ")\n" + ex.message;
	if (ex.externalMessage != "")
	{
		exceptionString += "\n(" + ex.externalSource + ") \"" + ex.externalMessage + "\"";
	}

	ConsoleLogger::LogError(exceptionString); // print the exception data as error to the console
}

int main()
{
	HephException::OnException = &HandleExceptions;
	File f("thisFileDoesNotExist.txt", FileOpenMode::Read);
	return 0;
}
```
With this we conclude our introduction to the HephAudio library. To learn more about the library and audio in general visit the [docs](/docs).
