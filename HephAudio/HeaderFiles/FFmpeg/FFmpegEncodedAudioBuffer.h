#pragma once
#include "HephAudioShared.h"
#include "EncodedAudioBuffer.h"
#include "FFmpegAudioShared.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief stores the audio data that's encoded via [FFmpeg](https://www.ffmpeg.org/).
	 * 
	 */
	class FFmpegEncodedAudioBuffer final : public EncodedAudioBuffer
	{
	private:
		using EncodedAudioBuffer::Resize;

	private:
		size_t frameCount;
		size_t extraDataSize;
		void* extraData;
		size_t blockAlign;

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
		size_t GetFrameCount() const;
		size_t GetBlockAlign() const;
		void SetBlockAlign(size_t blockAlign);
		size_t GetExtraDataSize() const;
		void* GetExtraData() const;
		void SetExtraData(void* pExtraData, size_t extraDataSize);
		void Add(AVPacket* packet, size_t frameCount);
		AVPacket** begin() const;
		AVPacket** end() const;
	};
}