#include "AudioObject.h"
#include "AudioException.h"
#include "AudioProcessor.h"

namespace HephAudio
{
	AudioObject::AudioObject()
		: filePath(""), name(""), pause(false), loopCount(1), volume(1.0), buffer(AudioBuffer()), frameIndex(0), queueName(""), queueIndex(0), queueDelay_ms(0.0)
		, OnRender(AudioEvent()), OnFinishedPlaying(AudioEvent())
	{
		this->OnRender += OnRenderHandler;
	}
	void AudioObject::OnRenderHandler(AudioEventArgs* pArgs, AudioEventResult* pResult)
	{
		AudioObject* pAudioObject = (AudioObject*)pArgs->pAudioObject;
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

		pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, pRenderArgs->renderFrameCount);
		pAudioObject->frameIndex += pRenderArgs->renderFrameCount;
		pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
	}
}