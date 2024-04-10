# Recording (Capturing)

When some amount of data is recorded (typically 10ms) the ``OnCapture`` [event](/docs/HephCommon/Event.md) is raised.<br>

Record for 5 seconds, then play it:
```c++
#include <iostream>
#include <chrono>
#include <Audio.h>
#include <ConsoleLogger.h>
#include <StringHelpers.h>
#include <AudioProcessor.h>

using namespace HephCommon;
using namespace HephAudio;

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

AudioObject* pAudioObject;
void HandleCapture(const EventParams& eventParams)
{
    AudioCaptureEventArgs* pCaptureArgs = (AudioCaptureEventArgs*)eventParams.pArgs;

    AudioProcessor::ConvertToInnerFormat(pCaptureArgs->captureBuffer);
    if (pAudioObject->buffer.FrameCount() == 0)
    {
        pAudioObject->buffer = pCaptureArgs->captureBuffer;
    }
    else
    {
        pAudioObject->buffer.Append(pCaptureArgs->captureBuffer);
    }
}

int main()
{
    // for printing errors
    HephException::OnException = &HandleExceptions;

    Audio audio;

    pAudioObject = audio.CreateAudioObject("render recorded data", 0, AudioFormatInfo()); 
    pAudioObject->OnRender = HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT;

    audio.InitializeCapture(HEPHAUDIO_CH_LAYOUT_MONO, 48000);
    audio.SetOnCaptureHandler(&HandleCapture);
   
   // record for 5 seconds
   std::this_thread::sleep_for(std::chrono::seconds(5));

    audio.StopCapturing();
    audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

    pAudioObject->isPaused = false;

    // stop program from exiting
    int x;
    std::cin >> x;

    audio.DestroyAudioObject(pAudioObject);

    return 0;
}
```

<br><br>
If you want to play while capturing, you have to use a [mutex](https://en.cppreference.com/w/cpp/thread/mutex) to synchronize the render and capture threads.
```c++
#include <iostream>
#include <chrono>
#include <thread>
#include <Audio.h>
#include <ConsoleLogger.h>
#include <StringHelpers.h>
#include <AudioProcessor.h>

using namespace HephCommon;
using namespace HephAudio;

void HandleExceptions(const EventParams &eventParams)
{
    const HephException &ex = ((HephExceptionEventArgs *)eventParams.pArgs)->exception; // get the exception data

    std::string exceptionString = "Error!\n" + ex.method + " (" + StringHelpers::ToHexString(ex.errorCode) + ")\n" + ex.message;
    if (ex.externalMessage != "")
    {
        exceptionString += "\n(" + ex.externalSource + ") \"" + ex.externalMessage + "\"";
    }

    ConsoleLogger::LogError(exceptionString); // print the exception data as error to the console
}

std::mutex renderCaptureMutex;
AudioObject *pAudioObject;
void HandleCapture(const EventParams &eventParams)
{
    std::lock_guard<std::mutex> lockGuard(renderCaptureMutex);

    AudioCaptureEventArgs *pCaptureArgs = (AudioCaptureEventArgs *)eventParams.pArgs;

    AudioProcessor::ConvertToInnerFormat(pCaptureArgs->captureBuffer);
    if (pAudioObject->buffer.FrameCount() == 0)
    {
        pAudioObject->buffer = pCaptureArgs->captureBuffer;
    }
    else
    {
        pAudioObject->buffer.Append(pCaptureArgs->captureBuffer);
    }
}

void HandleRender(const EventParams &eventParams)
{
    std::lock_guard<std::mutex> lockGuard(renderCaptureMutex);

    AudioRenderEventArgs *pRenderArgs = (AudioRenderEventArgs *)eventParams.pArgs;
    AudioRenderEventResult *pRenderResult = (AudioRenderEventResult *)eventParams.pResult;
    Native::NativeAudio *pNativeAudio = (Native::NativeAudio *)pRenderArgs->pNativeAudio;

    if (pAudioObject->buffer.FrameCount() >= pRenderArgs->renderFrameCount)
    {
        pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(0, pRenderArgs->renderFrameCount);
        AudioProcessor::ChangeChannelLayout(pRenderResult->renderBuffer, pNativeAudio->GetRenderFormat().channelLayout);

        pAudioObject->buffer.Cut(0, pRenderArgs->renderFrameCount);
        pAudioObject->frameIndex = 0;
    }

    pRenderResult->isFinishedPlaying = false;
}

int main()
{
    // for printing errors
    HephException::OnException = &HandleExceptions;

    Audio audio;

    pAudioObject = audio.CreateAudioObject("render recorded data", 0, AudioFormatInfo());
    pAudioObject->OnRender = &HandleRender;
    pAudioObject->isPaused = false;

    audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
    audio.InitializeCapture(HEPHAUDIO_CH_LAYOUT_MONO, 48000);
    audio.SetOnCaptureHandler(&HandleCapture);

    // stop program from exiting
    int x;
    std::cin >> x;

    audio.DestroyAudioObject(pAudioObject);

    return 0;
}
```
