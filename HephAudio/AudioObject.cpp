#include "AudioObject.h"
#include "AudioProcessor.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioObject::AudioObject()
		: filePath(""), name(""), isPaused(true), loopCount(1), volume(1.0), buffer(AudioBuffer()), frameIndex(0), OnRender(&AudioObject::OnRenderHandler) {}
	AudioObject::AudioObject(AudioObject&& rhs) noexcept
	{
		this->filePath = std::move(rhs.filePath);
		this->name = std::move(rhs.name);
		this->isPaused = rhs.isPaused;
		this->loopCount = rhs.loopCount;
		this->volume = rhs.volume;
		this->buffer = std::move(rhs.buffer);
		this->frameIndex = rhs.frameIndex;
		this->OnRender = rhs.OnRender;
		this->OnFinishedPlaying = rhs.OnFinishedPlaying;

		rhs.OnRender.ClearEventHandlers();
		rhs.OnRender.ClearUserArgs();
		rhs.OnFinishedPlaying.ClearEventHandlers();
		rhs.OnFinishedPlaying.ClearUserArgs();
	}
	AudioObject& AudioObject::operator=(AudioObject&& rhs) noexcept
	{
		this->filePath = std::move(rhs.filePath);
		this->name = std::move(rhs.name);
		this->isPaused = rhs.isPaused;
		this->loopCount = rhs.loopCount;
		this->volume = rhs.volume;
		this->buffer = std::move(rhs.buffer);
		this->frameIndex = rhs.frameIndex;
		this->OnRender = rhs.OnRender;
		this->OnFinishedPlaying = rhs.OnFinishedPlaying;

		rhs.OnRender.ClearEventHandlers();
		rhs.OnRender.ClearUserArgs();
		rhs.OnFinishedPlaying.ClearEventHandlers();
		rhs.OnFinishedPlaying.ClearUserArgs();

		return *this;
	}
	void AudioObject::OnRenderHandler(const EventParams& eventParams)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, pRenderArgs->renderFrameCount);
		pAudioObject->frameIndex += pRenderArgs->renderFrameCount;
		pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
	}
}