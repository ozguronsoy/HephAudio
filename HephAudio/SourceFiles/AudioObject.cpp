#include "AudioObject.h"
#include "AudioProcessor.h"
#include "NativeAudio/NativeAudio.h"
#include "HephMath.h"
#include "HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioObject::AudioObject()
		: id(Guid::GenerateNew()), filePath(""), name(""), isPaused(true), playCount(1), volume(1.0), frameIndex(0), OnRender(&AudioObject::DefaultRenderHandler) {}
	AudioObject::AudioObject(AudioObject&& rhs) noexcept
		: id(rhs.id), filePath(std::move(rhs.filePath)), name(std::move(rhs.name)), isPaused(rhs.isPaused)
		, playCount(rhs.playCount), volume(rhs.volume), buffer(std::move(rhs.buffer)), frameIndex(rhs.frameIndex)
		, OnRender(rhs.OnRender), OnFinishedPlaying(rhs.OnFinishedPlaying)
	{
		rhs.OnRender.ClearAll();
		rhs.OnFinishedPlaying.ClearAll();
	}
	AudioObject& AudioObject::operator=(AudioObject&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->id = rhs.id;
			this->filePath = std::move(rhs.filePath);
			this->name = std::move(rhs.name);
			this->isPaused = rhs.isPaused;
			this->playCount = rhs.playCount;
			this->volume = rhs.volume;
			this->buffer = std::move(rhs.buffer);
			this->frameIndex = rhs.frameIndex;
			this->OnRender = rhs.OnRender;
			this->OnFinishedPlaying = rhs.OnFinishedPlaying;

			rhs.OnRender.ClearAll();
			rhs.OnFinishedPlaying.ClearAll();
		}

		return *this;
	}
	heph_float AudioObject::GetPosition() const
	{
		const heph_float position = ((heph_float)this->frameIndex) / this->buffer.FrameCount();
		return HEPH_MATH_MIN(position, 1.0);
	}
	void AudioObject::SetPosition(heph_float position)
	{
		if (position > 1.0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioObject::SetPosition", "position must be between 0 and 1"));
			position = 1.0;
		}
		else if (position < 0.0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioObject::SetPosition", "position must be between 0 and 1"));
			position = 0.0;
		}
		this->frameIndex = position * this->buffer.FrameCount();
	}
	void AudioObject::DefaultRenderHandler(const EventParams& eventParams)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, pRenderArgs->renderFrameCount);
		pAudioObject->frameIndex += pRenderArgs->renderFrameCount;
		pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
	}
	void AudioObject::MatchFormatRenderHandler(const HephCommon::EventParams& eventParams)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;
		Native::NativeAudio* pNativeAudio = (Native::NativeAudio*)pRenderArgs->pNativeAudio;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		AudioFormatInfo renderFormat = pNativeAudio->GetRenderFormat();
		const size_t readFrameCount = ceil((heph_float)pRenderArgs->renderFrameCount * (heph_float)pAudioObject->buffer.FormatInfo().sampleRate / (heph_float)renderFormat.sampleRate);

		pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, readFrameCount + 1);

		AudioProcessor::ChangeSampleRate(pRenderResult->renderBuffer, renderFormat.sampleRate);
		if (pRenderResult->renderBuffer.FrameCount() != pRenderArgs->renderFrameCount)
		{
			pRenderResult->renderBuffer.Resize(pRenderArgs->renderFrameCount);
		}

		AudioProcessor::ChangeChannelLayout(pRenderResult->renderBuffer, renderFormat.channelLayout);

		pAudioObject->frameIndex += readFrameCount;
		pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
	}
}