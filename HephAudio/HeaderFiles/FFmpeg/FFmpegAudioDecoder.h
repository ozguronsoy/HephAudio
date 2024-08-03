#pragma once
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "IAudioDecoder.h"

namespace HephAudio
{
	class FFmpegAudioDecoder final : public IAudioDecoder
	{
	private:
		static constexpr size_t AUDIO_STREAM_INDEX_NOT_FOUND = -1;
	private:
		size_t fileDuration_frame;
		size_t audioStreamIndex;
		int64_t firstPacketPts;
		AVFormatContext* avFormatContext;
		AVCodecContext* avCodecContext;
		SwrContext* swrContext;
		AVFrame* avFrame;
		AVPacket* avPacket;
	public:
		FFmpegAudioDecoder();
		FFmpegAudioDecoder(const std::string& filePath);
		FFmpegAudioDecoder(FFmpegAudioDecoder&& rhs) noexcept;
		FFmpegAudioDecoder(const FFmpegAudioDecoder&) = delete;
		~FFmpegAudioDecoder();
		FFmpegAudioDecoder& operator=(const FFmpegAudioDecoder&) = delete;
		FFmpegAudioDecoder& operator=(FFmpegAudioDecoder&& rhs) noexcept;
		void ChangeFile(const std::string& newFilePath);
		void CloseFile();
		bool IsFileOpen() const;
		AudioFormatInfo GetOutputFormatInfo() const;
		size_t GetFrameCount() const;
		bool Seek(size_t frameIndex);
		AudioBuffer Decode();
		AudioBuffer Decode(size_t frameCount);
		AudioBuffer Decode(size_t frameIndex, size_t frameCount);
	private:
		void OpenFile(const std::string& filePath);
		int SeekFrame(size_t& frameIndex);
	};
}