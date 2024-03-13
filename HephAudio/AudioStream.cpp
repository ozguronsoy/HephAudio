#include "AudioStream.h"
#include "AudioCodecManager.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio, const StringBuffer& filePath)
		: pNativeAudio(pNativeAudio), file(filePath, FileOpenMode::Read), pFileFormat(nullptr)
		, pAudioCodec(nullptr), frameCount(0), pAudioObject(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::AudioStream", "pNativeAudio must not be nullptr."));
			this->Release(false);
		}

		if (this->file.IsOpen())
		{
			this->pFileFormat = FileFormats::AudioFileFormatManager::FindFileFormat(this->file);
			if (this->pFileFormat == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::AudioStream", "File format '" + this->file.FileExtension() + "' is not supported."));
				this->Release(false);
			}

			this->pAudioObject = pNativeAudio->CreateAudioObject("(Stream) " + this->file.FileName(), 0);
			this->formatInfo = this->pFileFormat->ReadAudioFormatInfo(this->file);
			this->frameCount = this->pFileFormat->FileFrameCount(this->file, this->formatInfo);

			this->pAudioCodec = Codecs::AudioCodecManager::FindCodec(formatInfo.formatTag);
			if (this->pAudioCodec == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AudioStream::AudioStream", "Unsupported audio codec."));
				this->Release(true);
			}

			this->pAudioObject->OnRender = &AudioStream::OnRender;
			this->pAudioObject->OnFinishedPlaying = &AudioStream::OnFinishedPlaying;
			this->pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);
			this->pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY, this);

#if defined(HEPHAUDIO_USE_FFMPEG)
			this->file.Close();
			this->ffmpegAudioDecoder.ChangeFile(filePath);
#endif
		}
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
	AudioStream::~AudioStream()
	{
		this->Release(true);
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
		return this->pAudioObject != nullptr ? Math::Min((heph_float)this->pAudioObject->frameIndex / this->frameCount, 1.0_hf) : 0;
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
			const heph_float srRatio = (heph_float)renderSampleRate / (heph_float)pStream->formatInfo.sampleRate;
			const size_t decodedBufferFrameCount = av_rescale(pStream->decodedBuffer.FrameCount(), pStream->formatInfo.sampleRate, renderSampleRate);
			const size_t readFrameCount = av_rescale(pRenderArgs->renderFrameCount, pStream->formatInfo.sampleRate, renderSampleRate);

			size_t remainingFrameCount{ 0 };
			if (pRenderArgs->renderFrameCount >= decodedBufferFrameCount)
			{
				remainingFrameCount = pRenderArgs->renderFrameCount - decodedBufferFrameCount;
			}

			if (decodedBufferFrameCount > 0)
			{
				pStream->decodedBuffer.Append(pStream->ffmpegAudioDecoder.DecodeWholePackets(remainingFrameCount));
			}
			else
			{
				pStream->decodedBuffer = pStream->ffmpegAudioDecoder.DecodeWholePackets(remainingFrameCount);
			}

			if (srRatio != 1.0)
			{
				pRenderResult->renderBuffer = AudioBuffer(pRenderArgs->renderFrameCount, renderFormat);
				for (size_t i = 0; i < pRenderArgs->renderFrameCount; i++)
				{
					const heph_float resampleIndex = i / srRatio;
					const heph_float rho = resampleIndex - floor(resampleIndex);

					if ((resampleIndex + 1) < pStream->decodedBuffer.FrameCount())
					{
						for (size_t j = 0; j < renderFormat.channelCount; j++)
						{
							pRenderResult->renderBuffer[i][j] = pStream->decodedBuffer[resampleIndex][j] * (1.0 - rho) + pStream->decodedBuffer[resampleIndex + 1.0][j] * rho;
						}
					}
				}
				pStream->decodedBuffer.Cut(0, readFrameCount);
			}
			else
			{
				if (pRenderResult->renderBuffer.FrameCount() == 0)
				{
					pRenderResult->renderBuffer = pStream->decodedBuffer.GetSubBuffer(0, remainingFrameCount);
				}
				else
				{
					pRenderResult->renderBuffer.Append(pStream->decodedBuffer.GetSubBuffer(0, remainingFrameCount));
				}
				pStream->decodedBuffer.Cut(0, remainingFrameCount);
			}


			pAudioObject->frameIndex += readFrameCount;
			pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pStream->frameCount;
#else
			if (pStream->file.IsOpen())
			{
				const AudioFormatInfo renderFormat = pNativeAudio->GetRenderFormat();
				const heph_float srRatio = (heph_float)renderFormat.sampleRate / (heph_float)pStream->formatInfo.sampleRate;
				const size_t readFrameCount = (heph_float)pRenderArgs->renderFrameCount / srRatio;

				if (srRatio != 1.0) // change sample rate
				{
					pRenderResult->renderBuffer = AudioBuffer(pRenderArgs->renderFrameCount, renderFormat);
					const AudioBuffer originalBuffer = pStream->pFileFormat->ReadFile(pStream->file, pStream->pAudioCodec, pStream->formatInfo, pAudioObject->frameIndex, pRenderArgs->renderFrameCount, &pRenderResult->isFinishedPlaying);

					for (size_t i = 0; i < pRenderArgs->renderFrameCount; i++)
					{
						const heph_float resampleIndex = i / srRatio;
						const heph_float rho = resampleIndex - floor(resampleIndex);

						for (size_t j = 0; j < pRenderResult->renderBuffer.FormatInfo().channelCount && (resampleIndex + 1) < originalBuffer.FrameCount(); j++)
						{
							pRenderResult->renderBuffer[i][j] = originalBuffer[resampleIndex][j] * (1.0 - rho) + originalBuffer[resampleIndex + 1.0][j] * rho;
						}
					}
				}

				AudioProcessor::ChangeNumberOfChannels(pRenderResult->renderBuffer, renderFormat.channelCount);

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