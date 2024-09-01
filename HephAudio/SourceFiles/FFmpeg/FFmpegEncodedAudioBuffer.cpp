#include "FFmpeg/FFmpegEncodedAudioBuffer.h"

using namespace HephCommon;

namespace HephAudio
{
	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer() : BufferBase<FFmpegEncodedAudioBuffer, AVPacket*>() {}

	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer(size_t size) : BufferBase<FFmpegEncodedAudioBuffer, AVPacket*>(size) {}

	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer(size_t size, BufferFlags flags) : BufferBase<FFmpegEncodedAudioBuffer, AVPacket*>(size, flags) {}

	FFmpegEncodedAudioBuffer::FFmpegEncodedAudioBuffer(FFmpegEncodedAudioBuffer&& rhs) noexcept : BufferBase<FFmpegEncodedAudioBuffer, AVPacket*>(std::move(rhs)) {}

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

	void FFmpegEncodedAudioBuffer::Release()
	{
		if (!this->IsEmpty())
		{
			for (AVPacket* packet : (*this))
			{
				if (packet != nullptr)
				{
					av_packet_free(&packet);
				}
			}
		}
		BufferBase<FFmpegEncodedAudioBuffer, AVPacket*>::Release();
	}

	void FFmpegEncodedAudioBuffer::Add(AVPacket* packet)
	{
		this->Resize(this->size + 1);
		(*this)[this->size - 1] = packet;
	}
}