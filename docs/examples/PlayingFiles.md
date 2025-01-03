# Playing Files

- [Play](#play)<br>
- [Load](#load)<br>
- [Loop](#loop)<br>
- [Match Format](#match-format)<br>
- [Stream](#stream)<br>
- [Playlist](#playlist)<br>
- [Play From Other Sources](#play-from-other-sources)


## Play

``Audio::Play`` method decodes the file into memory and starts playing it.
You need to call the ``Audio::InitializeRender`` method before playing any kind of audio data, this method allocates and initializes the resources needed to render.

```c++
#include <iostream>
#include <Audio.h>

using namespace Heph;
using namespace HephAudio;

int main()
{
    // print errors to the console
    Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

    Audio audio;
    audio.InitializeRender();
    
    audio.Play("path/file.wav");
    audio.Play("path/another_file.mp3");

    int x;
    std::cin >> x;

    return 0;
}
```

## Load

Another way of playing audio is the ``Audio::Load`` method, which decodes the file into memory but does not start playing. This is useful when you need to do processing before playing the file.

```c++
#include <iostream>
#include <Audio.h>

using namespace Heph;
using namespace HephAudio;

int main()
{
    // print errors to the console
    Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

    Audio audio;
    audio.InitializeRender();

    AudioObject* pobj = audio.Load("path/file.wav");
    // do some processing
    pobj->Resume();

    int x;
    std::cin >> x;

    return 0;
}
```

## Loop

To play files more than once, you can simply pass the number of times you want the file to be played as a parameter to the ``Play`` and ``Load`` methods.
```c++
audio.Play("path/file.wav", 5); // play 5 times
audio.Load("path/file.wav", HEPHAUDIO_INFINITE_LOOP); // play infinite times
```

You can also set the ``playCount`` parameter directly.
```c++
AudioObject* pobj = audio.Load("path/file.wav");
pobj->playCount = 3;
```

## Match Format

In both of the ``Audio::Play`` and ``Audio::Load`` methods, the sample rate and the channel layout of the audio data are not changed. Thus the rendering format used by the audio device may differ and cause distortions. There are two ways to prevent this.

First one is by loading the file, changing the sample rate and/or channel layout, then playing it.
```c++
#include <iostream>
#include <Audio.h>
#include <AudioEffects/Resampler.h>
#include <AudioEffects/ChannelMapper.h>

using namespace Heph;
using namespace HephAudio;

int main()
{
    // print errors to the console
    Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

    Audio audio;
    audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

    // the audio device may not be capable of rendering with the provided format
    // and will use the closest format it supports in such cases
    // so you need to get the format it actually uses
    const AudioFormatInfo& renderFormat = audio.GetRenderFormat();

    AudioObject* pobj = audio.Load("path/file.wav");

    // match the sample rate
    Resampler resampler(renderFormat.sampleRate);
    resampler.Process(pobj->buffer);

    // match channel layout
    ChannelMapper channelMapper(renderFormat.channelLayout);
    channelMapper.Process(pobj->buffer);

    pobj->Resume();

    int x;
    std::cin >> x;

    return 0;
}
```
This will apply the resampling and channel mapping to the entire audio data at once and may take some time.

The second way is by handling the ``AudioObject::OnRender`` event and doing the processing there. There is a bulit-in handler for converting to render format. 
```c++
#include <iostream>
#include <Audio.h>

using namespace Heph;
using namespace HephAudio;

int main()
{
	// print errors to the console
	Exception::OnException = HEPH_EXCEPTION_DEFAULT_HANDLER;

	Audio audio;
	audio.InitializeRender(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

	AudioObject* pobj = audio.Load("path/file.wav");

	pobj->OnRender = HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT;
	pobj->Resume();

	int x;
	std::cin >> x;

	return 0;
}
```

## Stream

To play files without decoding the whole file into memory you can use the ``AudioStream`` class, which decodes the portion of the file that will be rendered in real-time. This class handles the format matching by default.

```c++
#include <iostream>
#include <Audio.h>
#include <AudioStream.h>

using namespace Heph;
using namespace HephAudio;

int main()
{
    Audio audio;
    audio.InitializeRender();

    AudioStream stream(audio, "path/file.wav");
    stream.Start();

    int x;
    std::cin >> x;

    return 0;
}
```

## Playlist

You can also create a playlists using the ``AudioPlaylist`` class, which uses ``AudioStream`` internally.
```c++
#include <iostream>
#include <Audio.h>
#include <AudioPlaylist.h>

using namespace Heph;
using namespace HephAudio;

int main()
{
    Audio audio;
    audio.InitializeRender();

    AudioPlaylist playlist(audio, {"path/file.wav", "path/another_file.mp3"});
    playlist.Start();

    int x;
    std::cin >> x;

    return 0;
}
```

## Play From Other Sources

To play from other sources than audio files you can create an ``AudioObject`` manually and fill its buffer.

```c++
#include <iostream>
#include <Audio.h>
#include <Oscillators/SineWaveOscillator.h>

using namespace Heph;
using namespace HephAudio;

int main()
{
    Audio audio;
    audio.InitializeRender();

    const AudioFormatInfo& renderFormat = audio.GetRenderFormat();

    // create audio object with 5 seconds of data.
    const size_t dataSize = renderFormat.sampleRate * 5;
    AudioObject* pobj = audio.CreateAudioObject("sine wave", dataSize, renderFormat.channelLayout, renderFormat.sampleRate);
    
    // generate 500Hz sine wave
    SineWaveOscillator osc(HEPH_AUDIO_SAMPLE_MAX, 500, renderFormat.sampleRate, 0);
    for (size_t i = 0; i < pobj->buffer.FrameCount(); ++i)
    {
        const heph_audio_sample_t sample = osc[i];
        for (size_t j = 0; j < renderFormat.channelLayout.count; ++j)
        {
            pobj->buffer[i][j] = sample;
        }
    }

    pobj->Resume();

    int x;
    std::cin >> x;

    return 0;
}
```