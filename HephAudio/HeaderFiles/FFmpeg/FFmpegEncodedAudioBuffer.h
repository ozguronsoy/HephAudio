#pragma once
#include "HephAudioShared.h"
#include "EncodedAudioBuffer.h"
#include "FFmpegAudioShared.h"

namespace HephAudio
{
	class FFmpegEncodedAudioBuffer final : private EncodedAudioBuffer
	{
	public:
		using EncodedAudioBuffer::Size;
		using EncodedAudioBuffer::SizeAsByte;
		using EncodedAudioBuffer::GetAudioFormatInfo;
		using EncodedAudioBuffer::SetAudioFormatInfo;

	public:
		FFmpegEncodedAudioBuffer();
		explicit FFmpegEncodedAudioBuffer(const AudioFormatInfo& formatInfo);
		FFmpegEncodedAudioBuffer(const FFmpegEncodedAudioBuffer&) = delete;
		FFmpegEncodedAudioBuffer(FFmpegEncodedAudioBuffer&& rhs) noexcept;
		~FFmpegEncodedAudioBuffer();
		FFmpegEncodedAudioBuffer& operator=(const FFmpegEncodedAudioBuffer&) = delete;
		FFmpegEncodedAudioBuffer& operator=(FFmpegEncodedAudioBuffer&& rhs) noexcept;
		AVPacket*& operator[](size_t index) const;
		void Release() override;
		void Add(AVPacket* packet);
		AVPacket** begin() const;
		AVPacket** end() const;
	};
}