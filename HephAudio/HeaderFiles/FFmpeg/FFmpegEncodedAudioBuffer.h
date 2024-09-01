#pragma once
#include "HephAudioShared.h"
#include "Buffers/BufferBase.h"
#include "FFmpegAudioShared.h"

namespace HephAudio
{
	class FFmpegEncodedAudioBuffer final : private HephCommon::BufferBase<FFmpegEncodedAudioBuffer, AVPacket*>
	{
		// this is so stupid
		friend class HephCommon::BufferBase<FFmpegEncodedAudioBuffer, AVPacket*>;
	public:
		FFmpegEncodedAudioBuffer();
		explicit FFmpegEncodedAudioBuffer(size_t size);
		FFmpegEncodedAudioBuffer(size_t size, HephCommon::BufferFlags flags);
		FFmpegEncodedAudioBuffer(const FFmpegEncodedAudioBuffer&) = delete;
		FFmpegEncodedAudioBuffer(FFmpegEncodedAudioBuffer&& rhs) noexcept;
		~FFmpegEncodedAudioBuffer();
		FFmpegEncodedAudioBuffer& operator=(const FFmpegEncodedAudioBuffer&) = delete;
		FFmpegEncodedAudioBuffer& operator=(FFmpegEncodedAudioBuffer&& rhs) noexcept;
		void Release() override;
		void Add(AVPacket* packet);
	};
}