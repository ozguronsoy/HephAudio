# Playing Files

There are 2 ways you can play a file. 
First one is by calling the ``Audio::Play`` method, which will read and decode the whole file into a buffer. 
Other way is by using the [AudioStream](/docs/HephAudio/AudioStream.md), which will decode the data before rendering. Hence will use much less memory.<br><br>


Playing files via ``Audio::Play`` method.
```c++
#include <iostream>
#include <Audio.h>
#include <ConsoleLogger.h>

using namespace HephCommon;
using namespace HephAudio;

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
    // for printing errors
    HephException::OnException = &HandleExceptions;

    Audio audio;
    
    // initialize rendering with the default device
    // param1: # of channels
    // param2: sample rate
    audio.InitializeRender(2, 48000);

    // play a file
    AudioObject* pAudioObject = audio.Play("some_path/some_file.wav");

    // convert the audio data to the render format before rendering
    // this will prevent distortion due to sample rate or # of channels mismatch.
    // not necessary when using Load instead of Play
    pAudioObject->OnRender = HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT;

    // stop program from exiting
    int x;
    std::cin >> x;

    return 0;
}
```
<br><br>

Playing files via ``AudioStream``.
```c++
#include <iostream>
#include <Audio.h>
#include <AudioStream.h>
#include <ConsoleLogger.h>

using namespace HephCommon;
using namespace HephAudio;

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
    // for printing errors
    HephException::OnException = &HandleExceptions;

    Audio audio;
    
    // initialize rendering with the default device
    // param1: # of channels
    // param2: sample rate
    audio.InitializeRender(2, 48000);

    AudioStream audioStream(audio, "some_path/some_file.wav");
    audioStream.Start();

    // stop program from exiting
    int x;
    std::cin >> x;

    return 0;
}
```
