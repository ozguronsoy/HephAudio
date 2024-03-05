#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "AudioFormatInfo.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"
#include "IAudioCodec.h"
#include <memory>

namespace HephAudio
{
	namespace FileFormats
	{
		class IAudioFileFormat
		{
		public:
			virtual ~IAudioFileFormat() = default;
			virtual HephCommon::StringBuffer Extensions() const = 0;
			virtual bool VerifyExtension(const HephCommon::StringBuffer& extension) const { return this->Extensions().Contains(extension); }
			virtual bool VerifySignature(const HephCommon::File& audioFile) const = 0;
			virtual size_t FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo) const = 0;
			virtual AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File& audioFile) const = 0;
			virtual AudioBuffer ReadFile(const HephCommon::File& audioFile) const = 0;
			virtual AudioBuffer ReadFile(const HephCommon::File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) const = 0;
			virtual bool SaveToFile(const HephCommon::StringBuffer& filePath, AudioBuffer& buffer, bool overwrite) const = 0;
		};
	}
}