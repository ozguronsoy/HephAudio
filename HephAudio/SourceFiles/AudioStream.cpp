#include "AudioStream.h"
#include "AudioProcessor.h"
#include "FFmpeg/FFmpegAudioShared.h"
#include "HephException.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio) : AudioStream(pNativeAudio, "") {}

	AudioStream::AudioStream(Audio& audio) : AudioStream(audio.GetNativeAudio()) {}

	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio, const std::string& filePath)
		: pNativeAudio(pNativeAudio), frameCount(0), pAudioObject(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::AudioStream", "pNativeAudio must not be nullptr."));
			this->Release(false);
		}

		if (filePath != "")
		{
			this->ChangeFile(filePath);
		}
	}

	AudioStream::AudioStream(Audio& audio, const std::string& filePath) : AudioStream(audio.GetNativeAudio(), filePath) {}

	AudioStream::AudioStream(AudioStream&& rhs) noexcept
		: pNativeAudio(rhs.pNativeAudio), formatInfo(rhs.formatInfo), frameCount(rhs.frameCount)
		, pAudioObject(rhs.pAudioObject), decodedBuffer(std::move(rhs.decodedBuffer))
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
		}

		rhs.pNativeAudio = nullptr;
		rhs.formatInfo = AudioFormatInfo();
		rhs.pAudioObject = nullptr;
	}
	AudioStream::~AudioStream()
	{
		this->Release(true);
	}
	AudioStream& AudioStream::operator=(AudioStream&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Release(true);

			this->pNativeAudio = rhs.pNativeAudio;
			this->formatInfo = rhs.formatInfo;
			this->frameCount = rhs.frameCount;
			this->pAudioObject = rhs.pAudioObject;
			this->decodedBuffer = std::move(rhs.decodedBuffer);
			if (this->pAudioObject != nullptr)
			{
				this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
				this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			}

			rhs.pNativeAudio = nullptr;
			rhs.formatInfo = AudioFormatInfo();
			rhs.frameCount = 0;
			rhs.pAudioObject = nullptr;
		}

		return *this;
	}
	Native::NativeAudio* AudioStream::GetNativeAudio() const
	{
		return this->pNativeAudio;
	}
	AudioObject* AudioStream::GetAudioObject() const
	{
		return this->pAudioObject;
	}
	const AudioFormatInfo& AudioStream::GetAudioFormatInfo() const
	{
		return this->formatInfo;
	}
	size_t AudioStream::GetFrameCount() const
	{
		return this->frameCount;
	}
	void AudioStream::ChangeFile(const std::string& newFilePath)
	{
		this->Stop();

		if (File::FileExists(newFilePath))
		{
			if (this->pAudioObject != nullptr)
			{
				this->pAudioObject->name = "(stream)" + File::GetFileName(newFilePath);
				this->pAudioObject->frameIndex = 0;
				this->pAudioObject->playCount = 1;
			}
			else
			{
				const AudioFormatInfo renderFormat = this->pNativeAudio->GetRenderFormat();
				this->pAudioObject = this->pNativeAudio->CreateAudioObject("(stream)" + File::GetFileName(newFilePath), 0, renderFormat.channelLayout, renderFormat.sampleRate);

				this->pAudioObject->OnRender = &AudioStream::OnRender;
				this->pAudioObject->OnFinishedPlaying = &AudioStream::OnFinishedPlaying;
				this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
				this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			}

			this->pNativeAudio->GetAudioDecoder()->ChangeFile(newFilePath);
			this->formatInfo = this->pNativeAudio->GetAudioDecoder()->GetOutputFormatInfo();
			this->frameCount = this->pNativeAudio->GetAudioDecoder()->GetFrameCount();
		}
	}
	void AudioStream::Start()
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->isPaused = false;
		}
	}
	void AudioStream::Stop()
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->isPaused = true;
		}
	}
	double AudioStream::GetPosition() const
	{
		const double position = ((double)this->pAudioObject->frameIndex) / this->frameCount;
		return this->pAudioObject != nullptr ? HEPH_MATH_MIN(position, (double)1.0) : 0;
	}
	void AudioStream::SetPosition(double position)
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->frameIndex = position * this->frameCount;
			this->pNativeAudio->GetAudioDecoder()->Seek(this->pAudioObject->frameIndex);
			this->decodedBuffer.Release();
		}
	}
	void AudioStream::Release()
	{
		this->Release(true);
	}
	void AudioStream::Release(bool destroyAudioObject)
	{
		if (this->pNativeAudio != nullptr)
		{
			this->pNativeAudio->GetAudioDecoder()->CloseFile();
			if (destroyAudioObject)
			{
				this->pNativeAudio->DestroyAudioObject(this->pAudioObject);
			}
		}

		this->decodedBuffer.Release();

		this->pAudioObject = nullptr;
		this->pNativeAudio = nullptr;
		this->formatInfo = AudioFormatInfo();
		this->frameCount = 0;
	}
	void AudioStream::OnRender(const EventParams& eventParams)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;
		Native::NativeAudio* pNativeAudio = (Native::NativeAudio*)pRenderArgs->pNativeAudio;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		AudioStream* pStream = (AudioStream*)eventParams.userEventArgs[HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY];
		if (pStream != nullptr)
		{
			const AudioFormatInfo renderFormat = pNativeAudio->GetRenderFormat();
			const uint32_t renderSampleRate = renderFormat.sampleRate;
			const size_t decodedBufferFrameCount = pStream->decodedBuffer.FrameCount();
			const size_t readFrameCount = av_rescale(pRenderArgs->renderFrameCount, pStream->formatInfo.sampleRate, renderSampleRate);
			const size_t minRequiredFrameCount = FFMAX(readFrameCount, pRenderArgs->renderFrameCount);

			size_t remainingFrameCount{ 0 };
			if (decodedBufferFrameCount > 0)
			{
				if (minRequiredFrameCount > decodedBufferFrameCount)
				{
					remainingFrameCount = minRequiredFrameCount - decodedBufferFrameCount;
					pStream->decodedBuffer.Append(pStream->pNativeAudio->GetAudioDecoder()->Decode(remainingFrameCount));
				}
			}
			else
			{
				remainingFrameCount = minRequiredFrameCount;
				pStream->decodedBuffer = pStream->pNativeAudio->GetAudioDecoder()->Decode(remainingFrameCount);
			}

			if (renderSampleRate != pStream->formatInfo.sampleRate)
			{
				pRenderResult->renderBuffer = pStream->decodedBuffer.SubBuffer(0, readFrameCount + 1);

				AudioProcessor::ChangeSampleRate(pRenderResult->renderBuffer, renderFormat.sampleRate);
				if (pRenderResult->renderBuffer.FrameCount() != pRenderArgs->renderFrameCount)
				{
					pRenderResult->renderBuffer.Resize(pRenderArgs->renderFrameCount);
				}

				pStream->decodedBuffer.Cut(0, readFrameCount);
			}
			else
			{
				pRenderResult->renderBuffer = pStream->decodedBuffer.SubBuffer(0, pRenderArgs->renderFrameCount);
				pStream->decodedBuffer.Cut(0, pRenderArgs->renderFrameCount);
			}

			AudioProcessor::ChangeChannelLayout(pRenderResult->renderBuffer, renderFormat.channelLayout);

			pAudioObject->frameIndex += readFrameCount;
			pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pStream->frameCount;
		}
	}
	void AudioStream::OnFinishedPlaying(const EventParams& eventParams)
	{
		AudioFinishedPlayingEventArgs* pFinishedPlayingEventArgs = (AudioFinishedPlayingEventArgs*)eventParams.pArgs;

		AudioStream* pStream = (AudioStream*)eventParams.userEventArgs[HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY];
		if (pStream != nullptr && pFinishedPlayingEventArgs->remainingLoopCount == 0)
		{
			pStream->Release(false);
		}
	}
}