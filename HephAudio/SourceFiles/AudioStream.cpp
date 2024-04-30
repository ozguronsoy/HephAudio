#include "AudioStream.h"
#include "AudioProcessor.h"
#include "AudioFormats/AudioFileFormatManager.h"
#include "HephException.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio) : pNativeAudio(pNativeAudio), frameCount(0), pAudioObject(nullptr) {}

	AudioStream::AudioStream(Audio& audio)
		: pNativeAudio(audio.GetNativeAudio()), frameCount(0), pAudioObject(nullptr) {}

	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio, const std::string& filePath) : pNativeAudio(pNativeAudio), frameCount(0), pAudioObject(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::AudioStream", "pNativeAudio must not be nullptr."));
			this->Release(false);
		}
		this->ChangeFile(filePath);
	}

	AudioStream::AudioStream(Audio& audio, const std::string& filePath) : AudioStream(audio.GetNativeAudio(), filePath) {}

	AudioStream::AudioStream(AudioStream&& rhs) noexcept
		: pNativeAudio(rhs.pNativeAudio), formatInfo(rhs.formatInfo), frameCount(rhs.frameCount)
		, pAudioObject(rhs.pAudioObject), ffmpegAudioDecoder(std::move(rhs.ffmpegAudioDecoder)), decodedBuffer(std::move(rhs.decodedBuffer))
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
			this->ffmpegAudioDecoder = std::move(rhs.ffmpegAudioDecoder);
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
			if (FileFormats::AudioFileFormatManager::FindFileFormat(newFilePath) == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::ChangeFile", "Unsupported codec."));
				return;
			}

			if (this->pAudioObject != nullptr)
			{
				this->pAudioObject->name = "(stream)" + File::GetFileName(newFilePath);
				this->pAudioObject->frameIndex = 0;
				this->pAudioObject->playCount = 1;
			}
			else
			{
				const AudioFormatInfo renderFormat = this->pNativeAudio->GetRenderFormat();
				this->pAudioObject = this->pNativeAudio->CreateAudioObject("(stream)" + File::GetFileName(newFilePath), 0, HEPHAUDIO_INTERNAL_FORMAT(renderFormat.channelLayout, renderFormat.sampleRate));

				this->pAudioObject->OnRender = &AudioStream::OnRender;
				this->pAudioObject->OnFinishedPlaying = &AudioStream::OnFinishedPlaying;
				this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
				this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			}

			this->ffmpegAudioDecoder.ChangeFile(newFilePath);
			this->formatInfo = this->ffmpegAudioDecoder.GetOutputFormatInfo();
			this->frameCount = this->ffmpegAudioDecoder.GetFrameCount();
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
	heph_float AudioStream::GetPosition() const
	{
		const heph_float position = ((heph_float)this->pAudioObject->frameIndex) / this->frameCount;
		return this->pAudioObject != nullptr ? HEPH_MATH_MIN(position, (heph_float)1.0) : 0;
	}
	void AudioStream::SetPosition(heph_float position)
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->frameIndex = position * this->frameCount;
			this->ffmpegAudioDecoder.Seek(this->pAudioObject->frameIndex);
			this->decodedBuffer.Release();
		}
	}
	void AudioStream::Release()
	{
		this->Release(true);
	}
	void AudioStream::Release(bool destroyAudioObject)
	{
		if (this->pNativeAudio != nullptr && destroyAudioObject)
		{
			this->pNativeAudio->DestroyAudioObject(this->pAudioObject);
		}

		this->ffmpegAudioDecoder.CloseFile();
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
					pStream->decodedBuffer.Append(pStream->ffmpegAudioDecoder.DecodeWholePackets(remainingFrameCount));
				}
			}
			else
			{
				remainingFrameCount = minRequiredFrameCount;
				pStream->decodedBuffer = pStream->ffmpegAudioDecoder.DecodeWholePackets(remainingFrameCount);
			}

			if (renderSampleRate != pStream->formatInfo.sampleRate)
			{
				pRenderResult->renderBuffer = pStream->decodedBuffer.GetSubBuffer(0, readFrameCount + 1);

				AudioProcessor::ChangeSampleRate(pRenderResult->renderBuffer, renderFormat.sampleRate);
				if (pRenderResult->renderBuffer.FrameCount() != pRenderArgs->renderFrameCount)
				{
					pRenderResult->renderBuffer.Resize(pRenderArgs->renderFrameCount);
				}

				pStream->decodedBuffer.Cut(0, readFrameCount);
			}
			else
			{
				pRenderResult->renderBuffer = pStream->decodedBuffer.GetSubBuffer(0, pRenderArgs->renderFrameCount);
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