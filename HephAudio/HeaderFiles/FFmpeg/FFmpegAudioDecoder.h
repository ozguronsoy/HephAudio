#pragma once
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "AudioBuffer.h"
#include <string>

namespace HephAudio
{
	class FFmpegAudioDecoder final
	{
	private:
		static constexpr size_t AUDIO_STREAM_INDEX_NOT_FOUND = -1;
	private:
		std::string audioFilePath;
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
		FFmpegAudioDecoder(const std::string& audioFilePath);
		FFmpegAudioDecoder(FFmpegAudioDecoder&& rhs) noexcept;
		FFmpegAudioDecoder(const FFmpegAudioDecoder&) = delete;
		~FFmpegAudioDecoder();
		FFmpegAudioDecoder& operator=(const FFmpegAudioDecoder&) = delete;
		FFmpegAudioDecoder& operator=(FFmpegAudioDecoder&& rhs) noexcept;
		void ChangeFile(const std::string& newAudioFilePath);
		void CloseFile();
		bool IsFileOpen() const;
		AudioFormatInfo GetOutputFormatInfo() const;
		size_t GetFrameCount() const;
		bool Seek(size_t frameIndex);
		AudioBuffer Decode();
		AudioBuffer Decode(size_t frameIndex, size_t frameCount);
		AudioBuffer DecodeWholePackets(size_t minFrameCount);
	private:
		void OpenFile(const std::string& audioFilePath);
		int SeekFrame(size_t& frameIndex);
	};
}