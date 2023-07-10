#include "RealTimeAudioBuffer.h"
#include "HephException.h"
#include "AudioCodecManager.h"

namespace HephAudio
{
	std::vector<RealTimeAudioBuffer*> RealTimeAudioBuffer::buffers = std::vector<RealTimeAudioBuffer*>();
	RealTimeAudioBuffer::RealTimeAudioBuffer(Native::NativeAudio* pNativeAudio, HephCommon::StringBuffer filePath)
		: pNativeAudio(pNativeAudio), pFile(new HephCommon::File(filePath, HephCommon::FileOpenMode::Read)), pFileFormat(FileFormats::AudioFileFormatManager::FindFileFormat(filePath)), pao(nullptr)
	{
		if (this->pNativeAudio == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "RealTimeAudioBuffer::RealTimeAudioBuffer", "Native audio must not be nullptr."));
			this->Release(false);
		}

		if (this->pFileFormat == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_invalid_argument, "RealTimeAudioBuffer::RealTimeAudioBuffer", "File format '" + this->pFile->FileExtension() + "' is not supported."));
			this->Release(false);
		}

		this->pao = pNativeAudio->CreateAO("(RT) " + this->pFile->FileName(), 0);
		this->formatInfo = this->pFileFormat->ReadAudioFormatInfo(this->pFile);
		this->pao->buffer.frameCount = this->pFileFormat->FileFrameCount(this->pFile, this->formatInfo);

		this->pAudioCodec = Codecs::AudioCodecManager::FindCodec(formatInfo.formatTag);
		if (this->pAudioCodec == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephCommon::HephException(HephCommon::HephException::ec_fail, "RealTimeAudioBuffer::RealTimeAudioBuffer", "Unsupported audio codec."));
			this->Release(true);
		}

		this->pao->OnRender = &RealTimeAudioBuffer::OnRender;
		this->pao->OnFinishedPlaying = &RealTimeAudioBuffer::OnFinishedPlaying;

		RealTimeAudioBuffer::buffers.push_back(this);
	}
	RealTimeAudioBuffer::RealTimeAudioBuffer(RealTimeAudioBuffer&& rhs) noexcept
		: pFile(rhs.pFile), pFileFormat(rhs.pFileFormat), pAudioCodec(rhs.pAudioCodec), pNativeAudio(rhs.pNativeAudio), pao(rhs.pao), formatInfo(rhs.formatInfo)
	{
		rhs.pFile = nullptr;
		rhs.pFileFormat = nullptr;
		rhs.pAudioCodec = nullptr;
		rhs.pNativeAudio = nullptr;
		rhs.pao = nullptr;
		rhs.formatInfo = AudioFormatInfo();
		RealTimeAudioBuffer::RemoveFromVector(&rhs);
	}
	RealTimeAudioBuffer::~RealTimeAudioBuffer()
	{
		this->Release(true);
	}
	RealTimeAudioBuffer& RealTimeAudioBuffer::operator=(RealTimeAudioBuffer&& rhs) noexcept
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
		RealTimeAudioBuffer::RemoveFromVector(&rhs);

		RealTimeAudioBuffer::buffers.push_back(this);
		return *this;
	}
	HephCommon::File* RealTimeAudioBuffer::GetFile() const noexcept
	{
		return this->pFile;
	}
	FileFormats::IAudioFileFormat* RealTimeAudioBuffer::GetFileFormat() const noexcept
	{
		return this->pFileFormat;
	}
	Codecs::IAudioCodec* RealTimeAudioBuffer::GetAudioCodec() const noexcept
	{
		return this->pAudioCodec;
	}
	std::shared_ptr<AudioObject> RealTimeAudioBuffer::GetAudioObject() const noexcept
	{
		return this->pao;
	}
	const AudioFormatInfo& RealTimeAudioBuffer::GetAudioFormatInfo() const noexcept
	{
		return this->formatInfo;
	}
	void RealTimeAudioBuffer::Release() noexcept
	{
		this->Release(true);
	}
	void RealTimeAudioBuffer::Release(bool destroyAO) noexcept
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

		RealTimeAudioBuffer::RemoveFromVector(this);
	}
	void RealTimeAudioBuffer::RemoveFromVector(RealTimeAudioBuffer* pRtab) noexcept
	{
		for (size_t i = 0; i < RealTimeAudioBuffer::buffers.size(); i++)
		{
			if (pRtab == RealTimeAudioBuffer::buffers[i])
			{
				RealTimeAudioBuffer::buffers.erase(RealTimeAudioBuffer::buffers.begin() + i);
				return;
			}
		}
	}
	RealTimeAudioBuffer* RealTimeAudioBuffer::FindRTAB(const AudioObject* pAudioObject)
	{
		for (size_t i = 0; i < RealTimeAudioBuffer::buffers.size(); i++)
		{
			if (pAudioObject == RealTimeAudioBuffer::buffers[i]->pao.get())
			{
				return RealTimeAudioBuffer::buffers[i];
			}
		}
		return nullptr;
	}
	void RealTimeAudioBuffer::OnRender(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult)
	{
		AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
		AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

		RealTimeAudioBuffer* pRtab = RealTimeAudioBuffer::FindRTAB((AudioObject*)pRenderArgs->pAudioObject);
		if (pRtab != nullptr)
		{
			pRenderResult->renderBuffer = pRtab->pFileFormat->ReadFile(pRtab->pFile, pRtab->pAudioCodec, pRtab->formatInfo, pRtab->pao->frameIndex, pRenderArgs->renderFrameCount, &pRenderResult->isFinishedPlaying);
			pRtab->pao->frameIndex += pRenderArgs->renderFrameCount;
		}
	}
	void RealTimeAudioBuffer::OnFinishedPlaying(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult)
	{
		AudioFinishedPlayingEventArgs* pFinishedPlayingEventArgs = (AudioFinishedPlayingEventArgs*)pArgs;

		RealTimeAudioBuffer* pRtab = RealTimeAudioBuffer::FindRTAB((AudioObject*)pFinishedPlayingEventArgs->pAudioObject);
		if (pRtab != nullptr && pFinishedPlayingEventArgs->remainingLoopCount == 0)
		{
			pRtab->Release(false);
		}
	}
}