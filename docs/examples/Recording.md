# Recording (Capturing)

When some amount of data is recorded (typically 10ms) the ``OnCapture`` event is raised.<br>

Record for 5 seconds, then play it:

```c++
#include <iostream>
#include <chrono>
#include <Audio.h>
#include <AudioProcessor.h>

using namespace Heph;
using namespace HephAudio;

AudioObject* pAudioObject;
void HandleCapture(const EventParams& eventParams)
{
    AudioCaptureEventArgs* pCaptureArgs = (AudioCaptureEventArgs*)eventParams.pArgs;
    pAudioObject->buffer.Append(pCaptureArgs->captureBuffer);
}

int main()
{
    // for printing errors
    Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

    Audio audio;

    pAudioObject = audio.CreateAudioObject("render recorded data", 0, HEPHAUDIO_CH_LAYOUT_STEREO, 48000); 
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
#include <AudioProcessor.h>
#include <AudioEvents/AudioRenderEventArgs.h>
#include <AudioEvents/AudioRenderEventResult.h>

using namespace Heph;
using namespace HephAudio;

std::mutex renderCaptureMutex;
AudioObject *pAudioObject;
void HandleCapture(const EventParams &eventParams)
{
    std::lock_guard<std::mutex> lockGuard(renderCaptureMutex);

    AudioCaptureEventArgs *pCaptureArgs = (AudioCaptureEventArgs *)eventParams.pArgs;
    pAudioObject->buffer.Append(pCaptureArgs->captureBuffer);
}

void HandleRender(const EventParams &eventParams)
{
    std::lock_guard<std::mutex> lockGuard(renderCaptureMutex);

    AudioRenderEventArgs *pRenderArgs = (AudioRenderEventArgs *)eventParams.pArgs;
    AudioRenderEventResult *pRenderResult = (AudioRenderEventResult *)eventParams.pResult;

    if (pAudioObject->buffer.FrameCount() >= pRenderArgs->renderFrameCount)
    {
        pRenderResult->renderBuffer = pAudioObject->buffer.SubBuffer(0, pRenderArgs->renderFrameCount);
        AudioProcessor::ChangeChannelLayout(pRenderResult->renderBuffer, pRenderArgs->pNativeAudio->GetRenderFormat().channelLayout);

        pAudioObject->buffer.Cut(0, pRenderArgs->renderFrameCount);
        pAudioObject->frameIndex = 0;
    }

    pRenderResult->isFinishedPlaying = false;
}

int main()
{
    // for printing errors
    Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

    Audio audio;

    pAudioObject = audio.CreateAudioObject("render recorded data", 0, HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
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
