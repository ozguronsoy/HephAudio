# Playing Files

There are 2 ways you can play a file. 
First one is by calling the ``Audio::Play`` method, which will read and decode the whole file into a buffer. 
Other way is by using the ``AudioStream``, which will decode the data before rendering. Hence will use much less memory.<br><br>


Playing files via ``Audio::Play`` method.

```c++
#include <iostream>
#include <Audio.h>

using namespace Heph;
using namespace HephAudio;

int main()
{
    // for printing errors
    Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

    Audio audio;
    
    // initialize rendering with the default device and format
    audio.InitializeRender();

    // play a file once
    AudioObject* pAudioObject = audio.Play("some_path/some_file.wav", 1, true);

    // convert the audio data to the render format before rendering
    // this will prevent distortion due to sample rate or channel layout mismatch.
    // not necessary when using Load instead of Play
    pAudioObject->OnRender = HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT;
    
    // start playing
    pAudioObject->isPaused = false;

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

using namespace Heph;
using namespace HephAudio;

int main()
{
    // for printing errors
    Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

    Audio audio;
    
    // initialize rendering with the default device
    // param1: channel layout
    // param2: sample rate
    audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

    AudioStream audioStream(audio, "some_path/some_file.wav");
    audioStream.Start();

    // stop program from exiting
    int x;
    std::cin >> x;

    return 0;
}
```
