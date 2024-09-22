#include "FFmpeg/FFmpegEncodedAudioBuffer.h"

using namespace Heph;

namespace HephAudio
{
	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer()
		: EncodedAudioBuffer(), frameCount(0), blockAlign(0),
		extraDataSize(0), extraData(nullptr) {}

	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer(const AudioFormatInfo& formatInfo)
		: EncodedAudioBuffer(formatInfo), frameCount(0), blockAlign(0),
		extraDataSize(0), extraData(nullptr) {}

	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer(FFmpegEncodedAudioBuffer&& rhs) noexcept
		: EncodedAudioBuffer(std::move(rhs)), frameCount(rhs.frameCount), blockAlign(rhs.blockAlign),
		extraDataSize(rhs.extraDataSize), extraData(rhs.extraData)
	{
		rhs.frameCount = 0;
		rhs.extraDataSize = 0;
		rhs.extraData = nullptr;
		rhs.blockAlign = 0;
	}

	FFmpegEncodedAudioBuffer::~FFmpegEncodedAudioBuffer()
	{
		this->Release();
	}

	FFmpegEncodedAudioBuffer& FFmpegEncodedAudioBuffer::operator=(FFmpegEncodedAudioBuffer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->Release();

			this->pData = rhs.pData;
			this->size = rhs.size;
			this->frameCount = rhs.frameCount;
			this->extraDataSize = rhs.extraDataSize;
			this->extraData = rhs.extraData;
			this->blockAlign = rhs.blockAlign;

			rhs.pData = nullptr;
			rhs.size = 0;
			rhs.frameCount = 0;
			rhs.extraDataSize = 0;
			rhs.extraData = nullptr;
			rhs.blockAlign = 0;
		}
		return *this;
	}

	AVPacket*& FFmpegEncodedAudioBuffer::operator[](size_t index) const
	{
		return ((AVPacket**)this->pData)[index];
	}

	void FFmpegEncodedAudioBuffer::Release()
	{
		if (this->extraData != nullptr)
		{
			av_free(this->extraData);
			this->extraData = nullptr;
		}

		this->frameCount = 0;
		this->extraDataSize = 0;
		this->blockAlign = 0;

		if (!this->IsEmpty())
		{
			for (AVPacket*& packet : (*this))
			{
				if (packet != nullptr)
				{
					av_packet_free(&packet);
				}
			}
		}

		EncodedAudioBuffer::Release();
	}

	size_t FFmpegEncodedAudioBuffer::GetFrameCount() const
	{
		return this->frameCount;
	}

	size_t FFmpegEncodedAudioBuffer::GetBlockAlign() const
	{
		return this->blockAlign;
	}

	void FFmpegEncodedAudioBuffer::SetBlockAlign(size_t blockAlign)
	{
		this->blockAlign = blockAlign;
	}

	size_t FFmpegEncodedAudioBuffer::GetExtraDataSize() const
	{
		return this->extraDataSize;
	}

	void* FFmpegEncodedAudioBuffer::GetExtraData() const
	{
		return this->extraData;
	}

	void FFmpegEncodedAudioBuffer::SetExtraData(void* pExtraData, size_t extraDataSize)
	{
		if (this->extraData != nullptr)
		{
			av_free(this->extraData);
			this->extraData = nullptr;
		}
		this->extraDataSize = 0;

		if (pExtraData != nullptr && extraDataSize != 0)
		{
			this->extraData = av_malloc(extraDataSize + AV_INPUT_BUFFER_PADDING_SIZE);
			if (this->extraData == nullptr)
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, Exception(HEPH_EC_INSUFFICIENT_MEMORY, HEPH_FUNC, "Insufficient memory"));
			}
			(void)std::memcpy(this->extraData, pExtraData, extraDataSize);

			this->extraDataSize = extraDataSize;
		}

	}

	void FFmpegEncodedAudioBuffer::Add(AVPacket* packet, size_t frameCount)
	{
		const size_t oldSize = this->size;

		uint8_t* pTemp = (uint8_t*)std::realloc(this->pData, (oldSize + 1) * sizeof(AVPacket*));
		if (pTemp == nullptr)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, Exception(HEPH_EC_INSUFFICIENT_MEMORY, HEPH_FUNC, "Insufficient memory"));
		}

		this->pData = pTemp;
		this->size = oldSize + 1;
		this->frameCount += frameCount;

		(*this)[oldSize] = packet;
	}

	AVPacket** FFmpegEncodedAudioBuffer::begin() const
	{
		return ((AVPacket**)this->pData);
	}

	AVPacket** FFmpegEncodedAudioBuffer::end() const
	{
		return ((AVPacket**)this->pData) + this->size;
	}
}