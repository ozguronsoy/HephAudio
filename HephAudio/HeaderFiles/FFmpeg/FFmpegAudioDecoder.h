#pragma once
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "IAudioDecoder.h"
#include "FFmpegEncodedAudioBuffer.h"

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
		void ChangeFile(const std::string& newFilePath) override;
		void CloseFile() override;
		bool IsFileOpen() const override;
		AudioFormatInfo GetOutputFormatInfo() const override;
		size_t GetFrameCount() const override;
		bool Seek(size_t frameIndex) override;
		AudioBuffer Decode() override;
		AudioBuffer Decode(size_t frameCount) override;
		AudioBuffer Decode(size_t frameIndex, size_t frameCount) override;
		AudioBuffer Decode(const EncodedAudioBuffer& encodedBuffer) override;
	private:
		void OpenFile(const std::string& filePath);
		int SeekFrame(size_t& frameIndex);
	};
}