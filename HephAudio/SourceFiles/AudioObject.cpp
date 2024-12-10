#include "AudioObject.h"
#include "NativeAudio/NativeAudio.h"
#include "AudioEffects/ChannelMapper.h"
#include "AudioEffects/Resampler.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "AudioEvents/AudioFinishedPlayingEventArgs.h"
#include "HephMath.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	AudioObject::AudioObject()
		: id(Guid::GenerateNew()), filePath(""), name(""), 
		isPaused(true), playCount(1), volume(1.0), frameIndex(0) 
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
		if (position < 0 || position > 1)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "position must be in the range of [0, 1]"));
		}
		this->frameIndex = position * this->buffer.FrameCount();
	}

	void AudioObject::Pause() 
	{
		this->isPaused = true;
	}

	void AudioObject::Resume()
	{
		this->isPaused = false;
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
		static Resampler resampler;
		static ChannelMapper channelMapper;

		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;

		const AudioFormatInfo& inputFormat = pRenderArgs->pAudioObject->buffer.FormatInfo();
		const AudioFormatInfo& renderFormat = pRenderArgs->pNativeAudio->GetRenderFormat();

		resampler.SetOutputSampleRate(renderFormat.sampleRate);
		channelMapper.SetTargetLayout(renderFormat.channelLayout);

		const size_t requiredFrameCount = resampler.CalculateRequiredFrameCount(pRenderArgs->renderFrameCount, inputFormat);
		const size_t advanceSize = resampler.CalculateAdvanceSize(pRenderArgs->renderFrameCount, inputFormat);

		pRenderResult->renderBuffer = pRenderArgs->pAudioObject->buffer.SubBuffer(pRenderArgs->pAudioObject->frameIndex, requiredFrameCount);
		
		resampler.Process(pRenderResult->renderBuffer);
		channelMapper.Process(pRenderResult->renderBuffer);

		pRenderArgs->pAudioObject->frameIndex += advanceSize;
		pRenderResult->isFinishedPlaying = pRenderArgs->pAudioObject->frameIndex >= pRenderArgs->pAudioObject->buffer.FrameCount();
	}
}