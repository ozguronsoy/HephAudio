#pragma once
#if defined(HEPHAUDIO_USE_FFMPEG)
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "AudioBuffer.h"
#include "StringBuffer.h"

namespace HephAudio
{
	class FFmpegAudioEncoder final
	{
	private:
		HephCommon::StringBuffer audioFilePath;
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
		FFmpegAudioEncoder(const HephCommon::StringBuffer& audioFilePath, AudioFormatInfo outputFormatInfo, bool overwrite);
		FFmpegAudioEncoder(FFmpegAudioEncoder&& rhs) noexcept;
		FFmpegAudioEncoder(const FFmpegAudioEncoder&) = delete;
		~FFmpegAudioEncoder();
		FFmpegAudioEncoder& operator=(const FFmpegAudioEncoder&) = delete;
		FFmpegAudioEncoder& operator=(FFmpegAudioEncoder&& rhs) noexcept;
		void ChangeFile(const HephCommon::StringBuffer& newAudioFilePath, bool overwrite);
		void CloseFile();
		bool IsFileOpen() const;
		void Encode(const AudioBuffer& bufferToEncode);
		static void Transcode(const HephCommon::StringBuffer& inputFilePath, const HephCommon::StringBuffer& outputFilePath, bool overwrite);
		static void Transcode(const HephCommon::StringBuffer& inputFilePath, const HephCommon::StringBuffer& outputFilePath, AudioFormatInfo outputFormatInfo, bool overwrite);
	private:
		void OpenFile(const HephCommon::StringBuffer& audioFilePath, bool overwrite);
		static AVSampleFormat AFI2AVSF(FFmpegAudioEncoder* pEncoder, const AudioFormatInfo& afi);
		static uint32_t GetClosestSupportedSampleRate(const AVCodec* avCodec, uint32_t targetSampleRate);
		static AVSampleFormat GetClosestSupportedSampleFormat(FFmpegAudioEncoder* pEncoder, const AVCodec* avCodec, uint32_t targetFormatTag, uint16_t targetBitsPerSample);
		static void Transcode(void* pDecoder, FFmpegAudioEncoder& encoder);
	};
}
#endif