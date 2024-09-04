#pragma once
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "IAudioEncoder.h"
#include "AudioBuffer.h"
#include "FFmpegEncodedAudioBuffer.h"
#include <string>

namespace HephAudio
{
	class FFmpegAudioEncoder final : public IAudioEncoder
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
		FFmpegAudioEncoder();
		FFmpegAudioEncoder(const std::string& filePath, AudioFormatInfo outputFormatInfo, bool overwrite);
		FFmpegAudioEncoder(FFmpegAudioEncoder&& rhs) noexcept;
		FFmpegAudioEncoder(const FFmpegAudioEncoder&) = delete;
		~FFmpegAudioEncoder();
		FFmpegAudioEncoder& operator=(const FFmpegAudioEncoder&) = delete;
		FFmpegAudioEncoder& operator=(FFmpegAudioEncoder&& rhs) noexcept;
		void ChangeFile(const std::string& newAudioFilePath, bool overwrite) override;
		void CloseFile() override;
		bool IsFileOpen() const override;
		void Encode(const AudioBuffer& bufferToEncode) override;
		void Encode(const AudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer) override;
		void Transcode(const EncodedAudioBuffer& inputBuffer, EncodedAudioBuffer& outputBuffer) override;
		static void Transcode(const std::string& inputFilePath, const std::string& outputFilePath, bool overwrite);
		static void Transcode(const std::string& inputFilePath, const std::string& outputFilePath, AudioFormatInfo outputFormatInfo, bool overwrite);
	private:
		void OpenFile(const std::string& filePath, bool overwrite);
		static AVSampleFormat AFI2AVSF(FFmpegAudioEncoder* pEncoder, const AudioFormatInfo& afi);
		static uint32_t GetClosestSupportedSampleRate(const AVCodec* avCodec, uint32_t targetSampleRate);
		static AVSampleFormat GetClosestSupportedSampleFormat(FFmpegAudioEncoder* pEncoder, const AVCodec* avCodec, uint32_t targetFormatTag, uint16_t targetBitsPerSample);
		static void Transcode(void* pDecoder, FFmpegAudioEncoder& encoder);
	};
}