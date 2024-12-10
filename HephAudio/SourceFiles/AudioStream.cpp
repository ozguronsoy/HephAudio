#include "AudioStream.h"
#include "FFmpeg/FFmpegAudioShared.h"
#include "FFmpeg/FFmpegAudioDecoder.h"
#include "AudioEffects/ChannelMapper.h"
#include "AudioEffects/Resampler.h"
#include "AudioEvents/AudioRenderEventArgs.h"
#include "AudioEvents/AudioRenderEventResult.h"
#include "AudioEvents/AudioFinishedPlayingEventArgs.h"
#include "HephMath.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Exceptions/NotFoundException.h"

using namespace Heph;

namespace HephAudio
{
	AudioStream::AudioStream(std::shared_ptr<Native::NativeAudio> pNativeAudio) : AudioStream(pNativeAudio, "") {}

	AudioStream::AudioStream(Audio& audio) : AudioStream(audio.GetNativeAudio()) {}

	AudioStream::AudioStream(std::shared_ptr<Native::NativeAudio> pNativeAudio, const std::filesystem::path& filePath)
		: pNativeAudio(pNativeAudio), pAudioDecoder(new FFmpegAudioDecoder()), frameCount(0), pAudioObject(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "pNativeAudio must not be nullptr."));
		}

		if (filePath != "")
		{
			this->ChangeFile(filePath);
		}
	}

	AudioStream::AudioStream(Audio& audio, const std::filesystem::path& filePath) : AudioStream(audio.GetNativeAudio(), filePath) {}

	AudioStream::AudioStream(AudioStream&& rhs) noexcept
		: pNativeAudio(rhs.pNativeAudio), pAudioDecoder(rhs.pAudioDecoder),
		formatInfo(rhs.formatInfo), frameCount(rhs.frameCount),
		pAudioObject(rhs.pAudioObject), decodedBuffer(std::move(rhs.decodedBuffer))
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
		}

		rhs.pNativeAudio = nullptr;
		rhs.pAudioDecoder = nullptr;
		rhs.formatInfo = AudioFormatInfo();
		rhs.pAudioObject = nullptr;
	}

	AudioStream::~AudioStream()
	{
		this->Release();
	}

	AudioStream& AudioStream::operator=(AudioStream&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Release();

			this->pNativeAudio = rhs.pNativeAudio;
			this->pAudioDecoder = rhs.pAudioDecoder;
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
			rhs.pAudioDecoder = nullptr;
			rhs.formatInfo = AudioFormatInfo();
			rhs.frameCount = 0;
			rhs.pAudioObject = nullptr;
		}

		return *this;
	}

	std::shared_ptr<Native::NativeAudio> AudioStream::GetNativeAudio() const
	{
		return this->pNativeAudio;
	}

	std::shared_ptr<IAudioDecoder> AudioStream::GetAudioDecoder() const
	{
		return this->pAudioDecoder;
	}

	void AudioStream::SetAudioDecoder(std::shared_ptr<IAudioDecoder> pNewDecoder)
	{
		if (pNewDecoder == nullptr)
		{
			HEPH_RAISE_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Decoder cannot be null"));
			return;
		}
		this->pAudioDecoder = pNewDecoder;
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

	void AudioStream::ChangeFile(const std::filesystem::path& newFilePath)
	{
		const bool isPaused = (this->pAudioObject != nullptr) ? (this->pAudioObject->isPaused) : (true);

		this->Stop();
		this->CloseFile();

		if (newFilePath != "")
		{
			if (!std::filesystem::exists(newFilePath))
			{
				if (this->pAudioObject != nullptr)
				{
					this->pAudioObject->isPaused = isPaused;
				}
				HEPH_RAISE_AND_THROW_EXCEPTION(this, NotFoundException(HEPH_FUNC, "Could not find the file \"" + newFilePath.string() + "\""));
			}

			if (this->pAudioObject != nullptr)
			{
				this->pAudioObject->name = "(stream)" + newFilePath.filename().string();
				this->pAudioObject->filePath = newFilePath;
			}
			else
			{
				const AudioFormatInfo renderFormat = this->pNativeAudio->GetRenderFormat();
				this->pAudioObject = this->pNativeAudio->CreateAudioObject("(stream)" + newFilePath.filename().string(), 0, renderFormat.channelLayout, renderFormat.sampleRate);
				this->pAudioObject->filePath = newFilePath;

				this->pAudioObject->OnRender = &AudioStream::OnRender;
				this->pAudioObject->OnFinishedPlaying = &AudioStream::OnFinishedPlaying;
				this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
				this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			}

			this->pAudioDecoder->ChangeFile(newFilePath);
			this->formatInfo = this->pAudioDecoder->GetOutputFormatInfo();
			this->frameCount = this->pAudioDecoder->GetFrameCount();
			this->pAudioObject->isPaused = isPaused;
		}
	}

	void AudioStream::CloseFile()
	{
		if (this->pAudioObject != nullptr)
		{
			const bool isPaused = this->pAudioObject->isPaused;
			this->pAudioObject->Pause();

			this->pAudioObject->buffer.Release();
			this->pAudioObject->name = "";
			this->pAudioObject->filePath = "";
			this->pAudioObject->frameIndex = 0;
			this->pAudioObject->playCount = 1;
			this->pAudioObject->isPaused = isPaused;
		}

		if (this->pAudioDecoder != nullptr)
		{
			this->pAudioDecoder->CloseFile();
		}
	}

	void AudioStream::Start()
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->Resume();
		}
	}

	void AudioStream::Stop()
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->Pause();
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
			if (position < 0 || position > 1)
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "position must be in the range of [0, 1]"));
			}

			this->pAudioObject->frameIndex = position * this->frameCount;
			this->pAudioDecoder->Seek(this->pAudioObject->frameIndex);
			this->decodedBuffer.Release();
		}
	}

	void AudioStream::Release()
	{
		if (this->pNativeAudio != nullptr)
		{
			this->pAudioDecoder->CloseFile();
			this->pNativeAudio->DestroyAudioObject(this->pAudioObject);
		}

		this->decodedBuffer.Release();

		this->pAudioObject = nullptr;
		this->pNativeAudio = nullptr;
		this->pAudioDecoder = nullptr;
		this->formatInfo = AudioFormatInfo();
		this->frameCount = 0;
	}

	void AudioStream::OnRender(const EventParams& eventParams)
	{
		static Resampler resampler;
		static ChannelMapper channelMapper;

		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)eventParams.pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)eventParams.pResult;

		AudioStream* pStream = (AudioStream*)eventParams.userEventArgs[HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY];
		if (pStream != nullptr && pStream->pAudioDecoder != nullptr)
		{
			const AudioFormatInfo inputFormat = pStream->pAudioDecoder->GetOutputFormatInfo();
			const AudioFormatInfo& renderFormat = pRenderArgs->pNativeAudio->GetRenderFormat();

			resampler.SetOutputSampleRate(renderFormat.sampleRate);
			channelMapper.SetTargetLayout(renderFormat.channelLayout);

			const uint32_t renderSampleRate = renderFormat.sampleRate;
			const size_t decodedBufferFrameCount = pStream->decodedBuffer.FrameCount();
			const size_t requiredFrameCount = resampler.CalculateRequiredFrameCount(pRenderArgs->renderFrameCount, inputFormat);
			const size_t advanceSize = resampler.CalculateAdvanceSize(pRenderArgs->renderFrameCount, inputFormat);
			const size_t minRequiredFrameCount = FFMAX(requiredFrameCount, pRenderArgs->renderFrameCount);

			size_t remainingFrameCount{ 0 };
			if (decodedBufferFrameCount > 0)
			{
				if (minRequiredFrameCount > decodedBufferFrameCount)
				{
					remainingFrameCount = minRequiredFrameCount - decodedBufferFrameCount;
					pStream->decodedBuffer.Append(pStream->pAudioDecoder->Decode(remainingFrameCount));
				}
			}
			else
			{
				remainingFrameCount = minRequiredFrameCount;
				pStream->decodedBuffer = pStream->pAudioDecoder->Decode(remainingFrameCount);
			}

			if (renderSampleRate != pStream->formatInfo.sampleRate)
			{
				pRenderResult->renderBuffer = pStream->decodedBuffer.SubBuffer(0, requiredFrameCount);

				resampler.Process(pRenderResult->renderBuffer);
				if (pRenderResult->renderBuffer.FrameCount() != pRenderArgs->renderFrameCount)
				{
					pRenderResult->renderBuffer.Resize(pRenderArgs->renderFrameCount);
				}

				pStream->decodedBuffer.Cut(0, advanceSize);
			}
			else
			{
				pRenderResult->renderBuffer = pStream->decodedBuffer.SubBuffer(0, pRenderArgs->renderFrameCount);
				pStream->decodedBuffer.Cut(0, pRenderArgs->renderFrameCount);
			}

			channelMapper.Process(pRenderResult->renderBuffer);

			pRenderArgs->pAudioObject->frameIndex += advanceSize;
			pRenderResult->isFinishedPlaying = pRenderArgs->pAudioObject->frameIndex >= pStream->frameCount;
		}
	}

	void AudioStream::OnFinishedPlaying(const EventParams& eventParams)
	{
		AudioFinishedPlayingEventArgs* pFinishedPlayingEventArgs = (AudioFinishedPlayingEventArgs*)eventParams.pArgs;

		AudioStream* pStream = (AudioStream*)eventParams.userEventArgs[HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY];
		if (pStream != nullptr)
		{
			if (pFinishedPlayingEventArgs->remainingLoopCount == 0)
			{
				pStream->CloseFile();
			}
			else if (pStream->pAudioDecoder != nullptr)
			{
				pStream->pAudioDecoder->Seek(0);
			}
		}
	}
}