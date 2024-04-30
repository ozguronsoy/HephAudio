#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "AudioFormatInfo.h"
#include "AudioCodecs/IAudioCodec.h"
#include "FFmpeg/FFmpegAudioDecoder.h"
#include "FFmpeg/FFmpegAudioEncoder.h"
#include "File.h"
#include <string>
#include <memory>

namespace HephAudio
{
	namespace FileFormats
	{
		class IAudioFileFormat
		{
		protected:
			FFmpegAudioDecoder ffmpegAudioDecoder;
		public:
			virtual ~IAudioFileFormat() = default;
			virtual std::string Extensions() = 0;
			virtual bool VerifyExtension(const std::string& extension) { return this->Extensions().find_first_of(extension) != std::string::npos; }
			virtual bool VerifySignature(const HephCommon::File& audioFile) = 0;
			virtual size_t FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo) = 0;
			virtual AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File& audioFile) = 0;
			virtual AudioBuffer ReadFile(const HephCommon::File& audioFile) = 0;
			virtual AudioBuffer ReadFile(const HephCommon::File& audioFile, size_t frameIndex, size_t frameCount) = 0;
			virtual bool SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite) = 0;
		};
	}
}