# About
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
Let's start by playing a wav file with the default output device. First you need to initialize the audio class for rendering (playing) by calling the ``InitializeRender`` method. Then simply call the ``Play`` method with the full path to the wav file.
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
### Queueing Files
HephAudio allowes you to create queues by calling the ``Queue`` method. The files in the queue will be played one at a time in the given order. You can later add new files to the queue by calling the same method with the same queue name.
```
// queue name
// delay in ms after a file finished playing.
// a vector of file paths.
audio.Queue("my queue", 300, { "file1.wav", "file2.wav", "file3.wav" });
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
Most of the signal processing is done by the ``AudioProcessor`` class. To apply effects we must obtain the ``AudioBuffer`` that's storing the audio data. We can obtain this and any other data that's necessarry to play audio from the ``AudioObject`` that is returend by the ``Play``, ``Load`` and ``Queue`` methods.
Here is a sample code for playing the file in 2x speed without changing the pitch:
```
#include <stdio.h>
#include <string.h>
#include <Audio.h>
#include <AudioProcessor.h>

using namespace HephAudio; // so we don't have to write HephAudio:: every time

int main()
{
  Audio audio;
  audio.InitializeRender(nullptr, AudioFormatInfo(WAVE_FORMAT_PCM, 2, 16, 48000));

  std::shared_ptr<AudioObject> pAudioObject = audio.Load("some_path\\some_file.wav", true); // pause before applying effects.

  HannWindow window; // windows will be explained later in the docs, for now select a HannWindow
  AudioProcessor::ChangeSpeed(pAudioObject->buffer, 2.0, window); // plays in 2x speed without changing the pitch

  pAudioObject->pause = false; // unpause

  // prevent from exiting the app
  std::string s;
  std::cin >> s;

  return 0;
}
```
