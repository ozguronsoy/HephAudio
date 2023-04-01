#pragma once
#include "framework.h"
#include "IAudioFileFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		class AudioFileFormatManager final
		{
		private:
			static bool CompareExtensions(HephCommon::StringBuffer lhs, HephCommon::StringBuffer rhs);
		public:
			AudioFileFormatManager() = delete;
			AudioFileFormatManager(const AudioFileFormatManager&) = delete;
			AudioFileFormatManager& operator=(const AudioFileFormatManager&) = delete;
		public:
			static void RegisterFileFormat(IAudioFileFormat* format);
			static IAudioFileFormat* FindFileFormat(AudioFile& file);
			static IAudioFileFormat* FindFileFormat(HephCommon::StringBuffer filePath);
		};
	}
}