#pragma once
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "IAudioEncoder.h"
#include "AudioBuffer.h"
#include "FFmpegEncodedAudioBuffer.h"
#include <string>

/** @file */

namespace HephAudio
{
	/**
	 * @brief implements audio encoding via [FFmpeg](https://www.ffmpeg.org/).
	 * 
	 */
	class HEPH_API FFmpegAudioEncoder final : public IAudioEncoder
	{
	private:
		AudioFormatInfo outputFormatInfo;
		AVFormatContext* avFormatContext;
		AVIOContext* avIoContext;
		AVStream* avStream;
		AVCodecContext* avCodecContext;
		SwrContext* swrContext;
		AVFrame* avFrame;
		AVPacket* avPacket;

	public:
		/** @copydoc default_constructor */
		FFmpegAudioEncoder();
		
		/** 
		 * @copydoc constructor 
		 * 
		 * @param filePath path of the file that will be decoded.
		 * @param outputFormatInfo describes the output format.
		 * @param overwrite indicates whether to write over the file if already exists.
		 */
		FFmpegAudioEncoder(const std::filesystem::path& filePath, AudioFormatInfo outputFormatInfo, bool overwrite);

		/** @copydoc move_constructor */
		FFmpegAudioEncoder(FFmpegAudioEncoder&& rhs) noexcept;

		FFmpegAudioEncoder(const FFmpegAudioEncoder&) = delete;

		/** @copydoc destructor */
		~FFmpegAudioEncoder();

		FFmpegAudioEncoder& operator=(const FFmpegAudioEncoder&) = delete;
		FFmpegAudioEncoder& operator=(FFmpegAudioEncoder&& rhs) noexcept;
		void ChangeFile(const std::filesystem::path& newAudioFilePath, bool overwrite) override;
		void CloseFile() override;
		bool IsFileOpen() const override;
		void Encode(const AudioBuffer& bufferToEncode) override;
		void Encode(const AudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer) override;
		void Transcode(const EncodedAudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer) override;
		static void Transcode(const std::filesystem::path& inputFilePath, const std::filesystem::path& outputFilePath, bool overwrite);
		static void Transcode(const std::filesystem::path& inputFilePath, const std::filesystem::path& outputFilePath, AudioFormatInfo outputFormatInfo, bool overwrite);

	private:
		void OpenFile(const std::filesystem::path& filePath, bool overwrite);
		static void Transcode(void* pDecoder, FFmpegAudioEncoder& encoder);
	};
}