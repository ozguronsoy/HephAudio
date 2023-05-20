#include "AudioObject.h"
#include "AudioProcessor.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioObject::AudioObject()
		: filePath(""), name(""), pause(false), loopCount(1), volume(1.0), buffer(AudioBuffer()), frameIndex(0), queueName(""), queueIndex(0), queueDelay_ms(0.0)
		, OnRender(Event()), OnFinishedPlaying(Event())
	{
		this->OnRender += OnRenderHandler;
	}
	void AudioObject::OnRenderHandler(EventArgs* pArgs, EventResult* pResult)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, pRenderArgs->renderFrameCount);
		pAudioObject->frameIndex += pRenderArgs->renderFrameCount;
		pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
	}
}