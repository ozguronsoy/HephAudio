#pragma once
#if defined(HEPHAUDIO_USE_FFMPEG)
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "AudioBuffer.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"

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
	private:
		void OpenFile(const HephCommon::StringBuffer& audioFilePath, bool overwrite);
		AVSampleFormat AFI2AVSF(const AudioFormatInfo& afi) const;
		static uint32_t GetClosestSupportedSampleRate(const AVCodec* avCodec, uint32_t targetSampleRate);
		static AVSampleFormat GetClosestSupportedSampleFormat(FFmpegAudioEncoder* pEncoder, const AVCodec* avCodec, uint32_t targetFormatTag, uint16_t targetBitsPerSample);
		static void PrintSupportedSampleFormats(const AVCodec* avCodec);
	};
}
#endif