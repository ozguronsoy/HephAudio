#pragma once
#include "HephAudioShared.h"
#include "EncodedAudioBuffer.h"
#include "FFmpegAudioShared.h"

namespace HephAudio
{
	class FFmpegEncodedAudioBuffer final : EncodedAudioBuffer
	{
	public:
		FFmpegEncodedAudioBuffer();
		FFmpegEncodedAudioBuffer(const FFmpegEncodedAudioBuffer&) = delete;
		FFmpegEncodedAudioBuffer(FFmpegEncodedAudioBuffer&& rhs) noexcept;
		~FFmpegEncodedAudioBuffer();
		FFmpegEncodedAudioBuffer& operator=(const FFmpegEncodedAudioBuffer&) = delete;
		FFmpegEncodedAudioBuffer& operator=(FFmpegEncodedAudioBuffer&& rhs) noexcept;
		AVPacket*& operator[](size_t index) const;
		size_t Size() const;
		size_t SizeAsByte() const;
		void Release() override;
		void Add(AVPacket* packet);
		AVPacket** begin() const;
		AVPacket** end() const;
	};
}