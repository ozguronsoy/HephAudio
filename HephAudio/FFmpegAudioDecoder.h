#pragma once
#if defined(HEPHAUDIO_USE_FFMPEG)
#include "HephAudioShared.h"
#include "FFmpegAudioShared.h"
#include "AudioBuffer.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"

namespace HephAudio
{
	class FFmpegAudioDecoder final
	{
	private:
		static constexpr size_t AUDIO_STREAM_INDEX_NOT_FOUND = -1;
	private:
		HephCommon::StringBuffer audioFilePath;
		size_t fileDuration_frame;
		size_t audioStreamIndex;
		uint16_t channelCount;
		uint32_t sampleRate;
		AVFormatContext* avFormatContext;
		AVCodecContext* avCodecContext;
		SwrContext* swrContext;
		AVFrame* avFrame;
		AVPacket* avPacket;
	public:
		FFmpegAudioDecoder();
		FFmpegAudioDecoder(const HephCommon::StringBuffer& audioFilePath);
		FFmpegAudioDecoder(FFmpegAudioDecoder&& rhs) noexcept;
		FFmpegAudioDecoder(const FFmpegAudioDecoder&) = delete;
		~FFmpegAudioDecoder();
		FFmpegAudioDecoder& operator=(const FFmpegAudioDecoder&) = delete;
		FFmpegAudioDecoder& operator=(FFmpegAudioDecoder&& rhs) noexcept;
		void ChangeFile(const HephCommon::StringBuffer& newAudioFilePath);
		void CloseFile();
		bool IsFileOpen() const;
		AudioFormatInfo GetOutputFormat() const;
		size_t GetFrameCount() const;
		AudioBuffer Decode();
		AudioBuffer Decode(size_t frameIndex, size_t frameCount);
	private:
		void OpenFile(const HephCommon::StringBuffer& audioFilePath);
		int SeekFrame(size_t& frameIndex);
		AudioFormatInfo SF2AFI(uint16_t channelCount, uint32_t sampleRate, AVSampleFormat sampleFormat, bool& outIsPlanar) const;
	};
}
#endif