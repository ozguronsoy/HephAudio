#include "AudioStream.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "AudioCodecManager.h"
#include "AudioProcessor.h"

using namespace HephCommon;

namespace HephAudio
{
	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio, StringBuffer filePath)
		: pNativeAudio(pNativeAudio), file(filePath, FileOpenMode::Read),
		pFileFormat(this->file.IsOpen() ? FileFormats::AudioFileFormatManager::FindFileFormat(this->file) : nullptr),
		pAudioCodec(nullptr), pAudioObject(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::AudioStream", "Native audio must not be nullptr."));
			this->Release(false);
		}

		if (this->file.IsOpen())
		{
			if (this->pFileFormat == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::AudioStream", "File format '" + this->file.FileExtension() + "' is not supported."));
				this->Release(false);
			}

			this->pAudioObject = pNativeAudio->CreateAudioObject("(Stream) " + this->file.FileName(), 0);
			this->formatInfo = this->pFileFormat->ReadAudioFormatInfo(this->file);
			this->pAudioObject->buffer.frameCount = this->pFileFormat->FileFrameCount(this->file, this->formatInfo);

			this->pAudioCodec = Codecs::AudioCodecManager::FindCodec(formatInfo.formatTag);
			if (this->pAudioCodec == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AudioStream::AudioStream", "Unsupported audio codec."));
				this->Release(true);
			}

			this->pAudioObject->userEventArgs = this;
			this->pAudioObject->OnRender = &AudioStream::OnRender;
			this->pAudioObject->OnFinishedPlaying = &AudioStream::OnFinishedPlaying;
		}
	}
	AudioStream::AudioStream(Audio& audio, StringBuffer filePath) : AudioStream(audio.GetNativeAudio(), filePath) {}
	AudioStream::AudioStream(AudioStream&& rhs) noexcept
		: pNativeAudio(rhs.pNativeAudio), file(std::move(rhs.file)), pFileFormat(rhs.pFileFormat), pAudioCodec(rhs.pAudioCodec), formatInfo(rhs.formatInfo), pAudioObject(rhs.pAudioObject)
	{
		if (this->pAudioObject != nullptr)
		{
			this->pAudioObject->userEventArgs = this;
		}

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
			this->pAudioObject = rhs.pAudioObject;

			rhs.pNativeAudio = nullptr;
			rhs.pFileFormat = nullptr;
			rhs.pAudioCodec = nullptr;
			rhs.formatInfo = AudioFormatInfo();
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
	}
	void AudioStream::OnRender(EventArgs* pArgs, EventResult* pResult)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;
		Native::NativeAudio* pNativeAudio = (Native::NativeAudio*)pRenderArgs->pNativeAudio;
		AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

		AudioStream* pStream = (AudioStream*)pAudioObject->userEventArgs;
		if (pStream != nullptr && pStream->file.IsOpen())
		{
			const AudioFormatInfo renderFormat = pNativeAudio->GetRenderFormat();
			const heph_float srRatio = (heph_float)renderFormat.sampleRate / (heph_float)pStream->formatInfo.sampleRate;
			const size_t readFrameCount = (heph_float)pRenderArgs->renderFrameCount / srRatio;
			pRenderResult->renderBuffer = pStream->pFileFormat->ReadFile(pStream->file, pStream->pAudioCodec, pStream->formatInfo, pAudioObject->frameIndex, readFrameCount, &pRenderResult->isFinishedPlaying);

			if (srRatio != 1.0) // change sample rate
			{
				pRenderResult->renderBuffer.Resize(pRenderArgs->renderFrameCount);
				pRenderResult->renderBuffer.formatInfo.sampleRate = pRenderArgs->renderFrameCount;
				const AudioBuffer originalBuffer = pStream->pFileFormat->ReadFile(pStream->file, pStream->pAudioCodec, pStream->formatInfo, pAudioObject->frameIndex, pRenderArgs->renderFrameCount, &pRenderResult->isFinishedPlaying);

				for (size_t i = 0; i < pRenderArgs->renderFrameCount; i++)
				{
					const heph_float resampleIndex = i / srRatio;
					const heph_float rho = resampleIndex - floor(resampleIndex);

					for (size_t j = 0; j < pRenderResult->renderBuffer.formatInfo.channelCount && (resampleIndex + 1) < originalBuffer.frameCount; j++)
					{
						if (resampleIndex + 1.0 >= originalBuffer.frameCount) break;
						pRenderResult->renderBuffer[i][j] = originalBuffer[resampleIndex][j] * (1.0 - rho) + originalBuffer[resampleIndex + 1.0][j] * rho;
					}
				}
			}

			AudioProcessor::ChangeNumberOfChannels(pRenderResult->renderBuffer, renderFormat.channelCount);

			pAudioObject->frameIndex += readFrameCount;
		}
	}
	void AudioStream::OnFinishedPlaying(EventArgs* pArgs, EventResult* pResult)
	{
		AudioFinishedPlayingEventArgs* pFinishedPlayingEventArgs = (AudioFinishedPlayingEventArgs*)pArgs;

		AudioStream* pStream = (AudioStream*)((AudioObject*)pFinishedPlayingEventArgs->pAudioObject)->userEventArgs;
		if (pStream != nullptr && pFinishedPlayingEventArgs->remainingLoopCount == 0)
		{
			pStream->Release(false);
		}
	}
}