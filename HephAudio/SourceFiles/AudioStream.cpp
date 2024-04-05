#include "AudioStream.h"
#include "AudioProcessor.h"
#include "AudioCodecs/AudioCodecManager.h"
#include "HephException.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio)
		: pNativeAudio(pNativeAudio), pFileFormat(nullptr)
		, pAudioCodec(nullptr), frameCount(0), pAudioObject(nullptr) {}

	AudioStream::AudioStream(Audio& audio)
		: pNativeAudio(audio.GetNativeAudio()), pFileFormat(nullptr)
		, pAudioCodec(nullptr), frameCount(0), pAudioObject(nullptr) {}

	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio, const StringBuffer& filePath)
		: pNativeAudio(pNativeAudio), pFileFormat(nullptr)
		, pAudioCodec(nullptr), frameCount(0), pAudioObject(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::AudioStream", "pNativeAudio must not be nullptr."));
			this->Release(false);
		}
		this->ChangeFile(filePath);
	}

	AudioStream::AudioStream(Audio& audio, const StringBuffer& filePath) : AudioStream(audio.GetNativeAudio(), filePath) {}

	AudioStream::AudioStream(AudioStream&& rhs) noexcept
		: pNativeAudio(rhs.pNativeAudio), file(std::move(rhs.file)), pFileFormat(rhs.pFileFormat)
		, pAudioCodec(rhs.pAudioCodec), formatInfo(rhs.formatInfo), frameCount(rhs.frameCount), pAudioObject(rhs.pAudioObject)
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
		}

#if defined(HEPHAUDIO_USE_FFMPEG)
		this->ffmpegAudioDecoder = std::move(rhs.ffmpegAudioDecoder);
		this->decodedBuffer = std::move(rhs.decodedBuffer);
#endif

		rhs.pNativeAudio = nullptr;
		rhs.pFileFormat = nullptr;
		rhs.pAudioCodec = nullptr;
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
			this->file = std::move(rhs.file);
			this->pFileFormat = rhs.pFileFormat;
			this->pAudioCodec = rhs.pAudioCodec;
			this->formatInfo = rhs.formatInfo;
			this->frameCount = rhs.frameCount;
			this->pAudioObject = rhs.pAudioObject;
			if (this->pAudioObject != nullptr)
			{
				this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
				this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			}

#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder = std::move(rhs.ffmpegAudioDecoder);
			this->decodedBuffer = std::move(rhs.decodedBuffer);
#endif

			rhs.pNativeAudio = nullptr;
			rhs.pFileFormat = nullptr;
			rhs.pAudioCodec = nullptr;
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
	File* AudioStream::GetFile()
	{
		return &this->file;
	}
	FileFormats::IAudioFileFormat* AudioStream::GetFileFormat() const
	{
		return this->pFileFormat;
	}
	Codecs::IAudioCodec* AudioStream::GetAudioCodec() const
	{
		return this->pAudioCodec;
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
	void AudioStream::ChangeFile(const StringBuffer& newFilePath)
	{
		this->Stop();
		this->file.Close();

		this->file.Open(newFilePath, FileOpenMode::Read);
		if (this->file.IsOpen())
		{
			this->pFileFormat = FileFormats::AudioFileFormatManager::FindFileFormat(this->file);
			if (this->pFileFormat == nullptr)
			{
				this->Release(true);
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::ChangeFile", "File format '" + this->file.FileExtension() + "' is not supported."));
			}

			if (this->pAudioObject != nullptr)
			{
				this->pAudioObject->name = StringBuffer::Join(' ', { "(Stream)", this->file.FileName() });
				this->pAudioObject->frameIndex = 0;
				this->pAudioObject->playCount = 1;
			}
			else
			{
				const AudioFormatInfo renderFormat = this->pNativeAudio->GetRenderFormat();
				this->pAudioObject = this->pNativeAudio->CreateAudioObject(StringBuffer::Join(' ', { "(Stream)", this->file.FileName() }), 0, HEPHAUDIO_INTERNAL_FORMAT(renderFormat.channelLayout, renderFormat.sampleRate));

				this->pAudioObject->OnRender = &AudioStream::OnRender;
				this->pAudioObject->OnFinishedPlaying = &AudioStream::OnFinishedPlaying;
				this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
				this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			}


			this->formatInfo = this->pFileFormat->ReadAudioFormatInfo(this->file);
			this->frameCount = this->pFileFormat->FileFrameCount(this->file, this->formatInfo);

#if defined(HEPHAUDIO_USE_FFMPEG)
			this->file.Close();
			this->ffmpegAudioDecoder.ChangeFile(newFilePath);
#else
			this->pAudioCodec = Codecs::AudioCodecManager::FindCodec(this->formatInfo.formatTag);
			if (this->pAudioCodec == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AudioStream::ChangeFile", "Unsupported audio codec."));
				this->Release(true);
			}
#endif
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
		return this->pAudioObject != nullptr ? Math::Min((heph_float)this->pAudioObject->frameIndex / this->frameCount, (heph_float)1.0) : 0;
	}
	void AudioStream::SetPosition(heph_float position)
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->frameIndex = position * this->frameCount;

#if defined(HEPHAUDIO_USE_FFMPEG)
			this->ffmpegAudioDecoder.Seek(this->pAudioObject->frameIndex);
			this->decodedBuffer.Empty();
#endif
		}
	}
	void AudioStream::Release()
	{
		this->Release(true);
	}
	void AudioStream::Release(bool destroyAudioObject)
	{
		this->file.Close();

		if (this->pNativeAudio != nullptr && destroyAudioObject)
		{
			this->pNativeAudio->DestroyAudioObject(this->pAudioObject);
		}

		this->pAudioObject = nullptr;
		this->pFileFormat = nullptr;
		this->pAudioCodec = nullptr;
		this->pNativeAudio = nullptr;
		this->formatInfo = AudioFormatInfo();
		this->frameCount = 0;

#if defined(HEPHAUDIO_USE_FFMPEG)
		this->ffmpegAudioDecoder.CloseFile();
		this->decodedBuffer.Empty();
#endif
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
#if defined(HEPHAUDIO_USE_FFMPEG)
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
#else
			if (pStream->file.IsOpen())
			{
				const AudioFormatInfo renderFormat = pNativeAudio->GetRenderFormat();
				const heph_float srRatio = (heph_float)renderFormat.sampleRate / (heph_float)pStream->formatInfo.sampleRate;
				const size_t readFrameCount = (heph_float)pRenderArgs->renderFrameCount / srRatio;

				if (srRatio != 1)
				{
					pRenderResult->renderBuffer = pStream->pFileFormat->ReadFile(pStream->file, pStream->pAudioCodec, pStream->formatInfo, pAudioObject->frameIndex, pRenderArgs->renderFrameCount + 1, &pRenderResult->isFinishedPlaying);
					AudioProcessor::ChangeSampleRate(pRenderResult->renderBuffer, renderFormat.sampleRate);
					if (pRenderResult->renderBuffer.FrameCount() != pRenderArgs->renderFrameCount)
					{
						pRenderResult->renderBuffer.Resize(pRenderArgs->renderFrameCount);
					}
				}
				else
				{
					pRenderResult->renderBuffer = pStream->pFileFormat->ReadFile(pStream->file, pStream->pAudioCodec, pStream->formatInfo, pAudioObject->frameIndex, pRenderArgs->renderFrameCount, &pRenderResult->isFinishedPlaying);
				}

				AudioProcessor::ChangeChannelLayout(pRenderResult->renderBuffer, renderFormat.channelLayout);

				pAudioObject->frameIndex += readFrameCount;
			}
#endif
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