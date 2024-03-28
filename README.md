# Introduction
HephAudio is a cross-platform audio library that provides:
- Playing and recording audio data in Windows, Linux, iOS, macOS, and Android.
- Audio device enumeration and selection.
- Tools for storing and processing audio data with ease.
- Easy to use sound effects and filters.
- FFT (uses [PocketFFT](https://gitlab.mpcdf.mpg.de/mtr/pocketfft/tree/cpp)) for frequency analysis of the audio signals.
- Reading and writing WAV and AIFF files.
- More file format and codec support via [FFmpeg](https://ffmpeg.org/). You will need to enable this feature, and download an FFmpeg build. See [EnableFFmpeg](/docs/tutorials/EnableFFmpeg.md) for more information.

# Setup
### Visual Studio
1) Clone the repo.
2) Right click to your project, go to ``Configuration Properties -> C/C++ -> General -> Additional Including Directories`` and add the locations of the HephCommon and HephAudio header files.
3) Now right click the solution and go to ``Add -> Existing Project``, under the HephCommon folder select ``HephCommon.vcxitems`` to add to your project. Repeat the same process for HephAudio.
4) Right click to your project, ``Add -> Reference -> Shared Projects`` and check both HephAudio and HephCommon.
5) Visual studio marks some of the standard functions as unsafe and prevents from compiling by throwing errors. To fix this, right click to your project and go to ``Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions`` and add ``_CRT_SECURE_NO_WARNINGS``.

# Getting Started
### Playing Files
Let's start by playing a wav file with the default output device. First we need to initialize the audio class for rendering (playing) by calling the ``InitializeRender`` method. Then simply call the ``Play`` method with the full path to the wav file.
```c++
#include <iostream>
#include <string.h>
#include <Audio.h>

using namespace HephAudio; // so we don't have to write HephAudio:: every time

int main()
{
	Audio audio;

	// 2 -> # of channels
	// 48000 -> 48 kHz sampling rate.
	audio.InitializeRender(2, 48000);

	audio.Play("some_path\\some_file.wav");

	// prevent from exiting the app
	std::string s;
	std::cin >> s;
  
	return 0;
}
```
If the audio data we want to play does not have the same format as the one we specified when initializing render, the sound will come out distorted. To prevent that we can call the ``Load`` method instead. This method converts the audio data to our target format before playing.
```c++
audio.Load("some_path\\some_file.wav", false); // false = don't pause.
```
We can also do these convertions on the samples that are just about to be played so we don't have to wait for converting all the data before start playing. You can find more information on this in the documentation files.

### Recording
To record audio, first we need to initialize capturing just like before, this will start the recording process. You can access the recorded data via ``OnCapture`` event, which is invoked when some amount of data is captured (typically 10ms of audio data).
To add an event handler use either one of the following methods:
```c++
audio.SetOnCaptureHandler(&MyCallbackMethod); // Removes all the other event handlers, then adds the provided one
audio.AddOnCaptureHandler(&MyCallbackMethod); // Adds the provided event handler
```

After setting an event handler for the ``OnCapture`` event, we must cast the provided ``EventArgs`` pointer to the correct type to access the captured data and append it to an ``AudioBuffer`` we created. For more details about the ``EventArgs`` and ``EventResult`` visit the documentation.
When you are done recording you can either call ``StopCapturing()`` method to deinitialize capturing or ``PauseCapture(bool isPaused)`` method to prevent from raising the ``OnCapture`` event.

Sample code for recording audio for 5 seconds, saving it to a file, then playing the file:
```c++
#include <iostream>
#include <string.h>
#include <thread>
#include <chrono>
#include <Audio.h>

#define SAMPLE_RATE 48000
#define NUM_OF_CHANNELS 2

using namespace HephCommon;
using namespace HephAudio;

AudioBuffer recordedAudio;

void RecordAudio(const EventParams& eventParams) // the event handler method
{
	AudioCaptureEventArgs* pCaptureArgs = (AudioCaptureEventArgs*)eventParams.pArgs; // cast the args to capture event args

	if (recordedAudio == nullptr)
	{
		// first captured data
		recordedAudio = pCaptureArgs->captureBuffer;
	}
	else
	{
		// append the captured samples to the end of our buffer
		recordedAudio.Append(pCaptureArgs->captureBuffer);
	}
}

int main()
{
	Audio audio;

	// set an event handler for capturing
	audio.SetOnCaptureHandler(&RecordAudio);

	// initialize with default device
	audio.InitializeCapture(nullptr, AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, NUM_OF_CHANNELS, 16, SAMPLE_RATE));

	// record for 5 seconds, then stop capturing
	std::this_thread::sleep_for(std::chrono::seconds(5));
	audio.StopCapturing();

	// Save the recorded audio data to a file if it doesn't exist.
	audio.SaveToFile(recordedAudio, "file_path\\fila_name.wav", true);

	recordedAudio.Empty(); // dispose of the unnecessary data

	// play the recorded file.
	audio.InitializeRender(nullptr, AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, NUM_OF_CHANNELS, 16, SAMPLE_RATE));
	audio.Play("file_path\\fila_name.wav");

	// prevent from exiting the app
	std::string s;
	std::cin >> s;

	return 0;
}
```

### Device Enumeration
To get a list of available audio devices call the ``GetAudioDevices`` method.
```c++
std::vector<AudioDevice> renderDevices = audio.GetAudioDevices(AudioDeviceType::Render);    // Get only the devices that are capable of rendering
std::vector<AudioDevice> captureDevices = audio.GetAudioDevices(AudioDeviceType::Capture);  // Get only the devices that are capable of capturing
std::vector<AudioDevice> audioDevices = audio.GetAudioDevices(AudioDeviceType::All);        // Get all devices.
```
To get the default audio device, call the ``GetDefaultAudioDevice`` method.
```c++
AudioDevice defaultRenderDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Render);
AudioDevice defaultCaptureDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Capture);
```
After obtaining the audio devices, simply pass a pointer of the desired audio device to the ``InitializeRender`` method.
```c++
audio.InitializeRender(&renderDevices[0], AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, 2, 16, 48000));
```

### Applying Effects
Most of the signal processing is done by the ``AudioProcessor`` class. To apply effects we must obtain the ``AudioBuffer`` that's storing the audio data. We can obtain this and any other data that's necessarry to play audio from the ``AudioObject`` that is returned by the ``Play`` and ``Load`` methods.
Here is a sample code for playing the file in 2x speed without changing the pitch:
```c++
#include <iostream>
#include <string.h>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Windows/HannWindow.h>

using namespace HephAudio;

int main()
{
	Audio audio;
	audio.InitializeRender(nullptr, AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, 2, 16, 48000));

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

In this example, we will create a ``StringBuffer`` with 4 characters and we will try to access to its 5th element. This is going to throw an invalid argument exception.
```c++
#include <HephException.h>
#include <ConsoleLogger.h>

using namespace HephCommon;

void HandleExceptions(const EventParams& eventParams)
{
  const HephException& ex = ((HephExceptionEventArgs*)eventParams.pArgs)->exception; // get the exception data

  StringBuffer exceptionString = "Error!\n" + ex.method + " (" + StringBuffer::ToHexString(ex.errorCode) + ")\n" + ex.message;
  if (!ex.externalMessage.IsNullOrEmpty())
  {
	  exceptionString += "\n(" + ex.externalSource + ") \"" + ex.externalMessage + "\"";
  }

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
