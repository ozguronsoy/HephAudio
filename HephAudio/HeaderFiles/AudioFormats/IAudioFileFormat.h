#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "AudioFormatInfo.h"
#include "AudioCodecs/IAudioCodec.h"
#include "FFmpeg/FFmpegAudioDecoder.h"
#include "FFmpeg/FFmpegAudioEncoder.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"
#include <memory>

namespace HephAudio
{
	namespace FileFormats
	{
		class IAudioFileFormat
		{
#if defined(HEPHAUDIO_USE_FFMPEG)
		protected:
			FFmpegAudioDecoder ffmpegAudioDecoder;
#endif
		public:
			virtual ~IAudioFileFormat() = default;
			virtual HephCommon::StringBuffer Extensions() = 0;
			virtual bool VerifyExtension(const HephCommon::StringBuffer& extension) { return this->Extensions().Contains(extension); }
			virtual bool VerifySignature(const HephCommon::File& audioFile) = 0;
			virtual size_t FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo) = 0;
			virtual AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File& audioFile) = 0;
			virtual AudioBuffer ReadFile(const HephCommon::File& audioFile) = 0;
			virtual AudioBuffer ReadFile(const HephCommon::File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) = 0;
			virtual bool SaveToFile(const HephCommon::StringBuffer& filePath, AudioBuffer& buffer, bool overwrite) = 0;
		};
	}
}