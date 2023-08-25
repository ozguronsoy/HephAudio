#include "AudioStream.h"
#include "HephException.h"
#include "AudioCodecManager.h"

namespace HephAudio
{
	std::vector<AudioStream*> AudioStream::streams = std::vector<AudioStream*>();
	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio, HephCommon::StringBuffer filePath)
		: pNativeAudio(pNativeAudio), pFile(new HephCommon::File(filePath, HephCommon::FileOpenMode::Read)), pFileFormat(FileFormats::AudioFileFormatManager::FindFileFormat(filePath)), pao(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioStream::AudioStream", "Native audio must not be nullptr."));
			this->Release(false);
		}

		if (this->pFileFormat == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "AudioStream::AudioStream", "File format '" + this->pFile->FileExtension() + "' is not supported."));
			this->Release(false);
		}

		this->pao = pNativeAudio->CreateAO("(RT) " + this->pFile->FileName(), 0);
		this->formatInfo = this->pFileFormat->ReadAudioFormatInfo(this->pFile);
		this->pao->buffer.frameCount = this->pFileFormat->FileFrameCount(this->pFile, this->formatInfo);

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
	AudioStream::AudioStream(AudioStream&& rhs) noexcept
		: pFile(rhs.pFile), pFileFormat(rhs.pFileFormat), pAudioCodec(rhs.pAudioCodec), pNativeAudio(rhs.pNativeAudio), pao(rhs.pao), formatInfo(rhs.formatInfo)
	{
		rhs.pFile = nullptr;
		rhs.pFileFormat = nullptr;
		rhs.pAudioCodec = nullptr;
		rhs.pNativeAudio = nullptr;
		rhs.pao = nullptr;
		rhs.formatInfo = AudioFormatInfo();
		AudioStream::RemoveStream(&rhs);
	}
	AudioStream::~AudioStream()
	{
		this->Release(true);
	}
	AudioStream& AudioStream::operator=(AudioStream&& rhs) noexcept
	{
		this->Release(true);

		this->pFile = rhs.pFile;
		this->pFileFormat = rhs.pFileFormat;
		this->pAudioCodec = rhs.pAudioCodec;
		this->pNativeAudio = rhs.pNativeAudio;
		this->pao = rhs.pao;
		this->formatInfo = rhs.formatInfo;

		rhs.pFile = nullptr;
		rhs.pFileFormat = nullptr;
		rhs.pAudioCodec = nullptr;
		rhs.pNativeAudio = nullptr;
		rhs.pao = nullptr;
		rhs.formatInfo = AudioFormatInfo();
		AudioStream::RemoveStream(&rhs);

		AudioStream::streams.push_back(this);
		return *this;
	}
	HephCommon::File* AudioStream::GetFile() const noexcept
	{
		return this->pFile;
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
		if (this->pFile != nullptr)
		{
			delete this->pFile;
		}

		if (this->pao != nullptr && destroyAO)
		{
			this->pNativeAudio->DestroyAO(this->pao);
		}

		this->pFile = nullptr;
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
	void AudioStream::OnRender(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

		AudioStream* pStream = AudioStream::FindStream((AudioObject*)pRenderArgs->pAudioObject);
		if (pStream != nullptr)
		{
			pRenderResult->renderBuffer = pStream->pFileFormat->ReadFile(pStream->pFile, pStream->pAudioCodec, pStream->formatInfo, pStream->pao->frameIndex, pRenderArgs->renderFrameCount, &pRenderResult->isFinishedPlaying);
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
}