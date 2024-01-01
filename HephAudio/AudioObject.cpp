#include "AudioObject.h"
#include "AudioProcessor.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioObject::AudioObject()
		: filePath(""), name(""), isPaused(true), loopCount(1), volume(1.0), buffer(AudioBuffer()), frameIndex(0), userEventArgs(nullptr), OnRender(Event()), OnFinishedPlaying(Event())
	{
		this->OnRender += OnRenderHandler;
	}
	AudioObject::AudioObject(AudioObject&& rhs) noexcept
	{
		this->filePath = std::move(rhs.filePath);
		this->name = std::move(rhs.name);
		this->isPaused = rhs.isPaused;
		this->loopCount = rhs.loopCount;
		this->volume = rhs.volume;
		this->buffer = std::move(rhs.buffer);
		this->frameIndex = rhs.frameIndex;
		this->userEventArgs = rhs.userEventArgs;
		this->OnRender = rhs.OnRender;
		this->OnFinishedPlaying = rhs.OnFinishedPlaying;

		rhs.userEventArgs = nullptr;
		rhs.OnRender.Clear();
		rhs.OnFinishedPlaying.Clear();
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
		this->userEventArgs = rhs.userEventArgs;
		this->OnRender = rhs.OnRender;
		this->OnFinishedPlaying = rhs.OnFinishedPlaying;

		rhs.userEventArgs = nullptr;
		rhs.OnRender.Clear();
		rhs.OnFinishedPlaying.Clear();

		return *this;
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