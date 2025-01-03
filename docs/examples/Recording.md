# Recording (Capturing)

When some amount of data is recorded (typically 10ms) the ``OnCapture`` event is raised.<br>

Record for 5 seconds, then play it:

```c++
#include <iostream>
#include <chrono>
#include <Audio.h>

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
    audio.OnCapture = &HandleCapture;

    // record for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    audio.StopCapturing();
    audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

    pAudioObject->isPaused = false;

    int x;
    std::cin >> x;

    audio.DestroyAudioObject(pAudioObject);

    return 0;
}
```