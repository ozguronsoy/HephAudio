# Introduction
HephAudio is a cross-platform audio library that provides:
- Playing and recording audio data in Windows, Linux, iOS, MacOS, and Android.
- Audio device enumeration and selection.
- Tools for storing and processing audio data with ease.
- Easy to use sound effects and filters.
- FFT for frequency analysis of audio signals.
- Room impulse respnose simulation using source-image method.
- Reading and writing WAV and AIFF files.
- Decoding and encoding multiple audio codecs.

# Setup
### Visual Studio
1) Copy the HephAudio and HephCommon folders to your projects root folder.
2) Right click to your project, go to ``Configuration Properties -> C/C++ -> General -> Additional Including Directories`` and add the locations of the HephCommon and HephAudio header files.
4) Now right click the solution and go to ``Add -> Existing Project``, under the HephCommon folder select ``HephCommon.vcxitems`` to add to your project. Repeat the same process for HephAudio.
5) Right click to your project, ``Add -> Reference -> Shared Projects`` and check both HephAudio and HephCommon.
6) Visual studio marks some of the standard functions as unsafe and prevents from compiling by throwing errors. To fix this right click to your project and go to ``Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions`` and add ``_CRT_SECURE_NO_WARNINGS``.
### Android Studio
1) Copy the HephAudio and HephCommon folders to the same folder that your **CMAKE** file is in.
2) Add ``include_directories(HephCommon/HeaderFiles HephAudio)`` command to your **CMAKE** file.
3) Add ``file(GLOB HEPH_AUDIO HephCommon/SourceFiles/*.cpp HephAudio/*.cpp)`` command to your **CMAKE** file.
4) Add ``${HEPH_AUDIO}`` to your ***add_library*** command. Ex: ``add_library(your_app SHARED native-lib.cpp ${HEPH_AUDIO})``.
5) Link the necessary libraries; ``m``, ``log``, ``OpenSLES``, and ``aaudio``. Ex: ``target_link_libraries(your_app m log OpenSLES aaudio)``.

# Getting Started
### Playing Files
Let's start by playing a wav file with the default output device. First we need to initialize the audio class for rendering (playing) by calling the ``InitializeRender`` method. Then simply call the ``Play`` method with the full path to the wav file.
```
#include <stdio.h>
#include <string.h>
#include <Audio.h>

using namespace HephAudio; // so we don't have to write HephAudio:: every time

int main()
{
  Audio audio;

  // Activate rendering (playing)
  // nullptr -> default device
  // 2 -> # of channels
  // 16 -> 16 bit resolution.
  // 48000 -> 48kHz sampling rate.
  audio.InitializeRender(nullptr, AudioFormatInfo(WAVE_FORMAT_PCM, 2, 16, 48000));

  audio.Play("some_path\\some_file.wav");

  // prevent from exiting the app
  std::string s;
  std::cin >> s;
  
  return 0;
}
```
If the audio data we want to play does not have the same format as the one we specified when initializing render, the sound will come out distorted. To prevent that we can call the ``Load`` method instead. This method converts the audio data to our target format before playing.
```
audio.Load("some_path\\some_file.wav", false); // false = don't pause.
```
We can also do these convertions on the samples that are just about to be played so we don't have to wait for converting all the data before start playing. You can find more information on this in the documentation files.

### Recording
To record audio, first we need to initialize capturing just like before, this will start the recording process. You can access the recorded data via ``OnCapture`` event, which is invoked when some amount of data is captured (typically 10ms of audio data).
To add an event handler use either one of the following methods:
```
audio.SetOnCaptureHandler(&MyCallbackMethod); // Removes all the other event handlers, then adds the provided one
audio.AddOnCaptureHandler(&MyCallbackMethod); // Adds the provided event handler
```

After setting an event handler for the ``OnCapture`` event, we must cast the provided ``EventArgs`` pointer to the correct type to access the captured data and append it to an ``AudioBuffer`` we created. For more details about the ``EventArgs`` and ``EventResult`` visit the documentation.
When you are done recording you can either call ``StopCapturing()`` method to deinitialize capturing or ``PauseCapture(bool isPaused)`` method to prevent from raising the ``OnCapture`` event.

Sample code for recording audio for 5 seconds, saving it to a file, then playing the file:
```
#include <stdio.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <Audio.h>

#define SAMPLE_RATE 48000
#define NUM_OF_CHANNELS 2

using namespace HephAudio;

AudioBuffer recordedAudio(0, HEPHAUDIO_INTERNAL_FORMAT(NUM_OF_CHANNELS, SAMPLE_RATE)); // create an audio buffer with 0 frames

void RecordAudio(const EventParams& eventParams) // the event handler method
{
  AudioCaptureEventArgs* pCaptureArgs = (AudioCaptureEventArgs*)eventParams.pArgs; // cast the args to capture event args
  recordedAudio.Append(pCaptureArgs->captureBuffer); // append the captured samples to the end of our buffer
}

int main()
{
  Audio audio;

  audio.SetOnCaptureHandler(&RecordAudio); // set an event handler for capturing
  audio.InitializeCapture(nullptr, AudioFormatInfo(WAVE_FORMAT_PCM, NUM_OF_CHANNELS, 16, SAMPLE_RATE)); // initialize with default device

  // record for 5 seconds, then stop capturing
  std::this_thread::sleep_for(std::chrono::seconds(5));
  audio.StopCapturing();

  // Save the recorded audio data to a file if it doesn't exist.
  audio.SaveToFile("file_path\\fila_name.wav", false, recordedAudio);

  recordedAudio.Empty(); // dispose of the unnecessary data

  // play the recorded file.
  audio.InitializeRender(nullptr, AudioFormatInfo(WAVE_FORMAT_PCM, NUM_OF_CHANNELS, 16, SAMPLE_RATE));
  audio.Play("file_path\\fila_name.wav");

  // prevent from exiting the app
  std::string s;
  std::cin >> s;
  
  return 0;
}
```

### Device Enumeration
To get a list of available audio devices call the ``GetAudioDevices`` method.
```
std::vector<AudioDevice> renderDevices = audio.GetAudioDevices(AudioDeviceType::Render);    // Get only the devices that are capable of rendering
std::vector<AudioDevice> captureDevices = audio.GetAudioDevices(AudioDeviceType::Capture);  // Get only the devices that are capable of capturing
std::vector<AudioDevice> audioDevices = audio.GetAudioDevices(AudioDeviceType::All);        // Get all devices.
```
To get the default audio device, call the ``GetDefaultAudioDevice`` method.
```
AudioDevice defaultRenderDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Render);
AudioDevice defaultCaptureDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Capture);
```
After obtaining the audio devices, simply pass a pointer of the desired audio device to the ``InitializeRender`` method.
```
audio.InitializeRender(&renderDevices[0], AudioFormatInfo(WAVE_FORMAT_PCM, 2, 16, 48000));
```

### Applying Effects
Most of the signal processing is done by the ``AudioProcessor`` class. To apply effects we must obtain the ``AudioBuffer`` that's storing the audio data. We can obtain this and any other data that's necessarry to play audio from the ``AudioObject`` that is returned by the ``Play`` and ``Load`` methods.
Here is a sample code for playing the file in 2x speed without changing the pitch:
```
#include <stdio.h>
#include <string.h>
#include <Audio.h>
#include <AudioProcessor.h>

using namespace HephAudio;

int main()
{
  Audio audio;
  audio.InitializeRender(nullptr, AudioFormatInfo(WAVE_FORMAT_PCM, 2, 16, 48000));

  AudioObject* pAudioObject = audio.Load("some_path\\some_file.wav", true); // pause before applying effects.

  printf("applying sound effects...");
  
  HannWindow window; // windows will be explained later in the docs, for now select a HannWindow
  AudioProcessor::ChangeSpeed(pAudioObject->buffer, 2.0, window); // plays in 2x speed without changing the pitch

  printf("sound effects applied!");

  pAudioObject->pause = false; // unpause

  // prevent from exiting the app
  std::string s;
  std::cin >> s;

  return 0;
}
```
### Handling Exceptions
Every error that occurs while using the library raises an ``OnException`` event but only some exception will actually throw. Using this event we can log the exception details to a file or for this instance, simply print it to console.

In this example, we will create a ``StringBuffer`` with 4 characters and we will try to access to its 5th element. This is going to throw an invalid argument exception.
```
#include <HephException.h>
#include <ConsoleLogger.h>

using namespace HephCommon;

void HandleExceptions(const EventParams& eventParams)
{
  const HephException& ex = ((HephExceptionEventArgs*)eventParams.pArgs)->exception; // get the exception data

  StringBuffer exceptionString = "Error!\n" + ex.method + " (" + StringBuffer::ToHexString(ex.errorCode) + ")\n" + ex.message;
  ConsoleLogger::LogError(exceptionString); // print the exception data as error to the console
}

int main()
{
  HephException::OnException = &HandleExceptions;

  StringBuffer str = "text";

  // invalid argument, index out of bounds
  // this will first raise an OnException event, then will throw the exception.
  ConsoleLogger::LogInfo(str.c_at(4));

  return 0;
}
```
With this we conclude our introduction to the HephAudio library. To learn more about the library and audio in general visit the docs folder.
