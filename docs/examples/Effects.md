# Effects

- [Using Effects](#using-effects)<br>
- [Multithreaded Processing](#multithreaded-processing)<br>
- [Real Time Processing](#real-time-processing)


## Using Effects

To apply an effect, create an instance of the effect you want and call the ``Process`` method.

```c++
#include <iostream>
#include <Audio.h>
#include <AudioEffects/TimeStretcher.h>

using namespace HephAudio;

int main()
{
	Audio audio;
	audio.InitializeRender();

	AudioObject* pobj = audio.Load("path/file.wav");

	TimeStretcher effect;
	effect.SetSpeed(0.5);
	effect.Process(pobj->buffer);

	pobj->OnRender = HEPHAUDIO_RENDER_HANDLER_MATCH_FORMAT;
	pobj->Resume();

	std::string s;
	std::cin >> s;

	return 0;
}
```
> [!CAUTION]
> Some audio effects keep internal variables (state) so they can be applied in real-time.
> To use the same effect instance on multiple audio objects, call the ``ResetInternalState`` method.

```c++
effect.Process(pobj_1->buffer);
effect.ResetInternalState();
effect.Process(pobj_2->buffer);
```

## Multithreaded Processing

To use multithreaded processing just set the thread count using ``SetThreadCount`` method.

> [!IMPORTANT] 
> Not all effects support multithreaded processing, you can check if an effect supports it by calling the ``HasMTSupport`` method.

```c++
// use 2 threads to process
// pass 0 to use all of the hardware threads
effect.SetThreadCount(2);
effect.Process(pobj->buffer);
```

## Real Time Processing

You can apply effects in real-time by calling the same ``Process`` method.

> [!IMPORTANT] 
> Not all effects support real-time processing, you can check if an effect supports it by calling the ``HasRTSupport`` method.

```c++
#include <iostream>
#include <Audio.h>
#include <AudioEvents/AudioRenderEventArgs.h>
#include <AudioEvents/AudioRenderEventResult.h>
#include <Oscillators/SineWaveOscillator.h>
#include <AudioEffects/Tremolo.h>

#define AUDIO_EFFECT_KEY "effect"

using namespace Heph;
using namespace HephAudio;

void ApplyEffectHandler(const EventParams& params);

int main()
{
	Audio audio;
	audio.InitializeRender();

	Tremolo effect;
	effect.SetDepth(0.5);
	effect.SetOscillator(SineWaveOscillator(1.0, 5.0, audio.GetRenderFormat().sampleRate, 0.0));

	AudioObject* pobj = audio.Load("path/file.wav");

	pobj->OnRender.userEventArgs.Add(AUDIO_EFFECT_KEY, &effect);
	pobj->OnRender += &ApplyEffectHandler;

	pobj->Resume();

	std::string s;
	std::cin >> s;

	audio.DestroyAudioObject(pobj);

	return 0;
}

void ApplyEffectHandler(const EventParams& params)
{
	AudioRenderEventArgs* pArgs = (AudioRenderEventArgs*)params.pArgs;
	AudioRenderEventResult* pResult = (AudioRenderEventResult*)params.pResult;
	AudioEffect* effect = (AudioEffect*)params.userEventArgs[AUDIO_EFFECT_KEY];

	if (effect != nullptr)
	{
		// render buffer is filled by the previous handler (the default handler)
		effect->Process(pResult->renderBuffer);
	}
}
```

Some effects require more or less samples to process. 
In this case you need to handle the reading as well.
```c++
#include <iostream>
#include <Audio.h>
#include <AudioEvents/AudioRenderEventArgs.h>
#include <AudioEvents/AudioRenderEventResult.h>
#include <AudioEffects/Resampler.h>

#define AUDIO_EFFECT_KEY "effect"

using namespace Heph;
using namespace HephAudio;

void ApplyEffectHandler(const EventParams& params);

int main()
{
	Audio audio;
	audio.InitializeRender();

	Resampler effect;
	effect.SetOutputSampleRate(audio.GetRenderFormat().sampleRate * 2);

	AudioObject* pobj = audio.Load("path/file.wav");

	pobj->OnRender.userEventArgs.Add(AUDIO_EFFECT_KEY, &effect);
	pobj->OnRender = &ApplyEffectHandler;

	pobj->Resume();

	std::string s;
	std::cin >> s;

	audio.DestroyAudioObject(pobj);

	return 0;
}

void ApplyEffectHandler(const EventParams& params)
{
	AudioRenderEventArgs* pArgs = (AudioRenderEventArgs*)params.pArgs;
	AudioRenderEventResult* pResult = (AudioRenderEventResult*)params.pResult;
	AudioEffect* effect = (AudioEffect*)params.userEventArgs[AUDIO_EFFECT_KEY];

	if (effect != nullptr)
	{
		const AudioFormatInfo& objFormat = pArgs->pAudioObject->buffer.FormatInfo();

		const size_t requiredFrameCount = effect->CalculateRequiredFrameCount(pArgs->renderFrameCount, objFormat);
		pResult->renderBuffer = pArgs->pAudioObject->buffer.SubBuffer(pArgs->pAudioObject->frameIndex, requiredFrameCount);

		effect->Process(pResult->renderBuffer);

		// number of frames to advance may differ from the number of frames read
		pArgs->pAudioObject->frameIndex += effect->CalculateAdvanceSize(pArgs->renderFrameCount, objFormat);

		pResult->isFinishedPlaying = (pArgs->pAudioObject->frameIndex >= pArgs->pAudioObject->buffer.FrameCount());
	}
}
```