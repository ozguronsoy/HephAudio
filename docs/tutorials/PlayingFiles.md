# Playing Files

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
