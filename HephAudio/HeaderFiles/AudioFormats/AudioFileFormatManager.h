#pragma once
#include "HephAudioShared.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class AudioFileFormatManager final
		{
		public:
			AudioFileFormatManager() = delete;
			AudioFileFormatManager(const AudioFileFormatManager&) = delete;
			AudioFileFormatManager& operator=(const AudioFileFormatManager&) = delete;
		public:
			static void RegisterFileFormat(IAudioFileFormat* format);
			static IAudioFileFormat* FindFileFormat(const HephCommon::File& file);
			static IAudioFileFormat* FindFileFormat(const std::string& filePath);
		};
	}
}