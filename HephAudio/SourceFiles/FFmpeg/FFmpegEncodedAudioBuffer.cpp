#include "FFmpeg/FFmpegEncodedAudioBuffer.h"

using namespace HephCommon;

namespace HephAudio
{
	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer() : EncodedAudioBuffer() {}

	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer(const AudioFormatInfo& formatInfo) : EncodedAudioBuffer(formatInfo) {}

	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer(FFmpegEncodedAudioBuffer&& rhs) noexcept : EncodedAudioBuffer(std::move(rhs)) {}

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

			rhs.pData = nullptr;
			rhs.size = 0;
		}
		return *this;
	}

	AVPacket*& FFmpegEncodedAudioBuffer::operator[](size_t index) const
	{
		return ((AVPacket**)this->pData)[index];
	}

	void FFmpegEncodedAudioBuffer::Release()
	{
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

	void FFmpegEncodedAudioBuffer::Add(AVPacket* packet)
	{
		this->Resize(this->size + 1);
		(*this)[this->size - 1] = packet;
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