#include "AudioObject.h"
#include "AudioProcessor.h"
#include "NativeAudio/NativeAudio.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "AudioEvents/AudioFinishedPlayingEventArgs.h"
#include "HephMath.h"
#include "HephException.h"

using namespace Heph;

namespace HephAudio
{
	AudioObject::AudioObject()
		: id(Guid::GenerateNew()), filePath(""), name(""), isPaused(true), playCount(1), volume(1.0), frameIndex(0) 
	{
		this->OnRender = HEPHAUDIO_RENDER_HANDLER_DEFAULT;
	}

	AudioObject::AudioObject(AudioObject&& rhs) noexcept
		: id(rhs.id), filePath(std::move(rhs.filePath)), name(std::move(rhs.name)), isPaused(rhs.isPaused),
		playCount(rhs.playCount), volume(rhs.volume), buffer(std::move(rhs.buffer)), frameIndex(rhs.frameIndex),
		OnRender(rhs.OnRender), OnFinishedPlaying(rhs.OnFinishedPlaying)
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
	double AudioObject::GetPosition() const
	{
		const double position = ((double)this->frameIndex) / this->buffer.FrameCount();
		return HEPH_MATH_MIN(position, 1.0);
	}
	void AudioObject::SetPosition(double position)
	{
		if (position > 1.0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "position must be between 0 and 1"));
			position = 1.0;
		}
		else if (position < 0.0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "position must be between 0 and 1"));
			position = 0.0;
		}
		this->frameIndex = position * this->buffer.FrameCount();
	}
	void AudioObject::DefaultRenderHandler(const EventParams& eventParams)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;

		pRenderResult->renderBuffer = pRenderArgs->pAudioObject->buffer.SubBuffer(pRenderArgs->pAudioObject->frameIndex, pRenderArgs->renderFrameCount);
		pRenderArgs->pAudioObject->frameIndex += pRenderArgs->renderFrameCount;
		pRenderResult->isFinishedPlaying = pRenderArgs->pAudioObject->frameIndex >= pRenderArgs->pAudioObject->buffer.FrameCount();
	}
	void AudioObject::MatchFormatRenderHandler(const Heph::EventParams& eventParams)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;

		AudioFormatInfo renderFormat = pRenderArgs->pNativeAudio->GetRenderFormat();
		const size_t readFrameCount = ceil((double)pRenderArgs->renderFrameCount * (double)pRenderArgs->pAudioObject->buffer.FormatInfo().sampleRate / (double)renderFormat.sampleRate);

		pRenderResult->renderBuffer = pRenderArgs->pAudioObject->buffer.SubBuffer(pRenderArgs->pAudioObject->frameIndex, readFrameCount + 1);

		AudioProcessor::ChangeSampleRate(pRenderResult->renderBuffer, renderFormat.sampleRate);
		if (pRenderResult->renderBuffer.FrameCount() != pRenderArgs->renderFrameCount)
		{
			pRenderResult->renderBuffer.Resize(pRenderArgs->renderFrameCount);
		}

		AudioProcessor::ChangeChannelLayout(pRenderResult->renderBuffer, renderFormat.channelLayout);

		pRenderArgs->pAudioObject->frameIndex += readFrameCount;
		pRenderResult->isFinishedPlaying = pRenderArgs->pAudioObject->frameIndex >= pRenderArgs->pAudioObject->buffer.FrameCount();
	}
}