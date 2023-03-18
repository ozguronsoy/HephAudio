#pragma once
#include "framework.h"
#include "IAudioFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		class AudioFormatManager final
		{
		private:
			static bool CompareExtensions(StringBuffer lhs, StringBuffer rhs);
		public:
			AudioFormatManager() = delete;
			AudioFormatManager(const AudioFormatManager&) = delete;
			AudioFormatManager& operator=(const AudioFormatManager&) = delete;
		public:
			static void RegisterFormat(IAudioFormat* format);
			static IAudioFormat* FindAudioFormat(AudioFile& file);
			static IAudioFormat* FindAudioFormat(StringBuffer filePath);
		};
	}
}