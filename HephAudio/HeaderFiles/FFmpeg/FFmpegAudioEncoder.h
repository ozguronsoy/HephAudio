#pragma once
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "AudioBuffer.h"
#include "FFmpegEncodedAudioBuffer.h"
#include <string>

namespace HephAudio
{
	class FFmpegAudioEncoder final
	{
	private:
		std::string audioFilePath;
		AudioFormatInfo outputFormatInfo;
		AVFormatContext* avFormatContext;
		AVIOContext* avIoContext;
		AVStream* avStream;
		AVCodecContext* avCodecContext;
		SwrContext* swrContext;
		AVFrame* avFrame;
		AVPacket* avPacket;
	public:
		FFmpegAudioEncoder();
		FFmpegAudioEncoder(const std::string& audioFilePath, AudioFormatInfo outputFormatInfo, bool overwrite);
		FFmpegAudioEncoder(FFmpegAudioEncoder&& rhs) noexcept;
		FFmpegAudioEncoder(const FFmpegAudioEncoder&) = delete;
		~FFmpegAudioEncoder();
		FFmpegAudioEncoder& operator=(const FFmpegAudioEncoder&) = delete;
		FFmpegAudioEncoder& operator=(FFmpegAudioEncoder&& rhs) noexcept;
		void ChangeFile(const std::string& newAudioFilePath, bool overwrite);
		void CloseFile();
		bool IsFileOpen() const;
		void Encode(const AudioBuffer& bufferToEncode);
		static FFmpegEncodedAudioBuffer Encode(const AudioBuffer& bufferToEncode, const AudioFormatInfo& targetFormat);
		static FFmpegEncodedAudioBuffer Transcode(const FFmpegEncodedAudioBuffer& bufferToTranscode, const AudioFormatInfo& targetFormat);
		static void Transcode(const std::string& inputFilePath, const std::string& outputFilePath, bool overwrite);
		static void Transcode(const std::string& inputFilePath, const std::string& outputFilePath, AudioFormatInfo outputFormatInfo, bool overwrite);
	private:
		void OpenFile(const std::string& audioFilePath, bool overwrite);
		static AVSampleFormat AFI2AVSF(FFmpegAudioEncoder* pEncoder, const AudioFormatInfo& afi);
		static uint32_t GetClosestSupportedSampleRate(const AVCodec* avCodec, uint32_t targetSampleRate);
		static AVSampleFormat GetClosestSupportedSampleFormat(FFmpegAudioEncoder* pEncoder, const AVCodec* avCodec, uint32_t targetFormatTag, uint16_t targetBitsPerSample);
		static void Transcode(void* pDecoder, FFmpegAudioEncoder& encoder);
	};
}