#include "AudioStream.h"
#include "HephException.h"
#include "AudioCodecManager.h"

namespace HephAudio
{
	std::vector<AudioStream*> AudioStream::streams = std::vector<AudioStream*>();
	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio, HephCommon::StringBuffer filePath)
		: pNativeAudio(pNativeAudio), file(filePath, HephCommon::FileOpenMode::Read), pFileFormat(FileFormats::AudioFileFormatManager::FindFileFormat(this->file)), pao(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioStream::AudioStream", "Native audio must not be nullptr."));
			this->Release(false);
		}

		if (this->pFileFormat == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioStream::AudioStream", "File format '" + this->file.FileExtension() + "' is not supported."));
			this->Release(false);
		}

		this->pao = pNativeAudio->CreateAO("(Stream) " + this->file.FileName(), 0);
		this->formatInfo = this->pFileFormat->ReadAudioFormatInfo(this->file);
		this->pao->buffer.frameCount = this->pFileFormat->FileFrameCount(this->file, this->formatInfo);

		this->pAudioCodec = Codecs::AudioCodecManager::FindCodec(formatInfo.formatTag);
		if (this->pAudioCodec == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "AudioStream::AudioStream", "Unsupported audio codec."));
			this->Release(true);
		}

		this->pao->OnRender = &AudioStream::OnRender;
		this->pao->OnFinishedPlaying = &AudioStream::OnFinishedPlaying;

		AudioStream::streams.push_back(this);
	}
	AudioStream::~AudioStream()
	{
		this->Release(true);
	}
	HephCommon::File* AudioStream::GetFile() noexcept
	{
		return &this->file;
	}
	FileFormats::IAudioFileFormat* AudioStream::GetFileFormat() const noexcept
	{
		return this->pFileFormat;
	}
	Codecs::IAudioCodec* AudioStream::GetAudioCodec() const noexcept
	{
		return this->pAudioCodec;
	}
	std::shared_ptr<AudioObject> AudioStream::GetAudioObject() const noexcept
	{
		return this->pao;
	}
	const AudioFormatInfo& AudioStream::GetAudioFormatInfo() const noexcept
	{
		return this->formatInfo;
	}
	void AudioStream::Release() noexcept
	{
		this->Release(true);
	}
	void AudioStream::Release(bool destroyAO) noexcept
	{
		this->file.Close();

		if (destroyAO && this->pao != nullptr)
		{
			this->pNativeAudio->DestroyAO(this->pao);
		}

		this->pao = nullptr;
		this->pFileFormat = nullptr;
		this->pAudioCodec = nullptr;
		this->pNativeAudio = nullptr;
		this->formatInfo = AudioFormatInfo();

		AudioStream::RemoveStream(this);
	}
	void AudioStream::RemoveStream(AudioStream* pStream) noexcept
	{
		for (size_t i = 0; i < AudioStream::streams.size(); i++)
		{
			if (pStream == AudioStream::streams[i])
			{
				AudioStream::streams.erase(AudioStream::streams.begin() + i);
				return;
			}
		}
	}
	void AudioStream::OnRender(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

		AudioStream* pStream = AudioStream::FindStream((AudioObject*)pRenderArgs->pAudioObject);
		if (pStream != nullptr && pStream->file.IsOpen())
		{
			pRenderResult->renderBuffer = pStream->pFileFormat->ReadFile(pStream->file, pStream->pAudioCodec, pStream->formatInfo, pStream->pao->frameIndex, pRenderArgs->renderFrameCount, &pRenderResult->isFinishedPlaying);
			pStream->pao->frameIndex += pRenderArgs->renderFrameCount;
		}
	}
	void AudioStream::OnFinishedPlaying(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult)
	{
		AudioFinishedPlayingEventArgs* pFinishedPlayingEventArgs = (AudioFinishedPlayingEventArgs*)pArgs;

		AudioStream* pStream = AudioStream::FindStream((AudioObject*)pFinishedPlayingEventArgs->pAudioObject);
		if (pStream != nullptr && pFinishedPlayingEventArgs->remainingLoopCount == 0)
		{
			pStream->Release(false);
		}
	}
	AudioStream* AudioStream::FindStream(const AudioObject* pAudioObject)
	{
		for (size_t i = 0; i < AudioStream::streams.size(); i++)
		{
			if (pAudioObject == AudioStream::streams[i]->pao.get())
			{
				return AudioStream::streams[i];
			}
		}
		return nullptr;
	}
}