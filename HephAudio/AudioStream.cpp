#include "AudioStream.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include "AudioCodecManager.h"

using namespace HephCommon;

namespace HephAudio
{
	std::vector<AudioStream*> AudioStream::streams = std::vector<AudioStream*>();
	AudioStream::AudioStream(Native::NativeAudio* pNativeAudio, StringBuffer filePath)
		: pNativeAudio(pNativeAudio), file(filePath, FileOpenMode::Read), pFileFormat(FileFormats::AudioFileFormatManager::FindFileFormat(this->file)), pAudioObject(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "AudioStream::AudioStream", "Native audio must not be nullptr."));
			this->Release(false);
		}

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

		this->pAudioObject->OnRender = &AudioStream::OnRender;
		this->pAudioObject->OnFinishedPlaying = &AudioStream::OnFinishedPlaying;

		AudioStream::streams.push_back(this);
	}
	AudioStream::AudioStream(Audio& audio, StringBuffer filePath) : AudioStream(audio.GetNativeAudio(), filePath) {}
	AudioStream::AudioStream(AudioStream&& rhs) noexcept
		: pNativeAudio(rhs.pNativeAudio), file(std::move(rhs.file)), pFileFormat(rhs.pFileFormat), pAudioCodec(rhs.pAudioCodec), formatInfo(rhs.formatInfo), pAudioObject(rhs.pAudioObject)
	{
		rhs.pNativeAudio = nullptr;
		rhs.pFileFormat = nullptr;
		rhs.pAudioCodec = nullptr;
		rhs.formatInfo = AudioFormatInfo();
		rhs.pAudioObject = nullptr;

		AudioStream::RemoveStream(&rhs);
		AudioStream::streams.push_back(this);
	}
	AudioStream& AudioStream::operator=(AudioStream&& rhs) noexcept
	{
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

		AudioStream::RemoveStream(&rhs);
		AudioStream::streams.push_back(this);

		return *this;
	}
	AudioStream::~AudioStream()
	{
		this->Release(true);
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
	void AudioStream::Release(bool destroyAO)
	{
		this->file.Close();

		if (destroyAO && this->pAudioObject != nullptr)
		{
			this->pNativeAudio->DestroyAudioObject(this->pAudioObject);
		}

		this->pAudioObject = nullptr;
		this->pFileFormat = nullptr;
		this->pAudioCodec = nullptr;
		this->pNativeAudio = nullptr;
		this->formatInfo = AudioFormatInfo();

		AudioStream::RemoveStream(this);
	}
	void AudioStream::RemoveStream(AudioStream* pStream)
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
	void AudioStream::OnRender(EventArgs* pArgs, EventResult* pResult)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

		AudioStream* pStream = AudioStream::FindStream((AudioObject*)pRenderArgs->pAudioObject);
		if (pStream != nullptr && pStream->file.IsOpen())
		{
			pRenderResult->renderBuffer = pStream->pFileFormat->ReadFile(pStream->file, pStream->pAudioCodec, pStream->formatInfo, pStream->pAudioObject->frameIndex, pRenderArgs->renderFrameCount, &pRenderResult->isFinishedPlaying);
			pStream->pAudioObject->frameIndex += pRenderArgs->renderFrameCount;
		}
	}
	void AudioStream::OnFinishedPlaying(EventArgs* pArgs, EventResult* pResult)
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
			if (pAudioObject == AudioStream::streams[i]->pAudioObject)
			{
				return AudioStream::streams[i];
			}
		}
		return nullptr;
	}
}