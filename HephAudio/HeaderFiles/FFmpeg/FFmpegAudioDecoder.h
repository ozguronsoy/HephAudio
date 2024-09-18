#pragma once
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "IAudioDecoder.h"
#include "FFmpegEncodedAudioBuffer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief implements audio decoding via [FFmpeg](https://www.ffmpeg.org/).
	 * 
	 */
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
		/** @copydoc default_constructor */
		FFmpegAudioDecoder();

		/** 
		 * @copydoc constructor 
		 * 
		 * @param filePath path of the file that will be decoded.
		 */
		FFmpegAudioDecoder(const std::string& filePath);

		/** @copydoc move_constructor */
		FFmpegAudioDecoder(FFmpegAudioDecoder&& rhs) noexcept;

		FFmpegAudioDecoder(const FFmpegAudioDecoder&) = delete;

		/** @copydoc destructor */
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