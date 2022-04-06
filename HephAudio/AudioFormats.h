#pragma once
#include "IAudioFormat.h"
#include "WavFormat.h"
#include "AiffFormat.h"
#include "Mp3Format.h"

namespace HephAudio
{
	namespace Formats
	{
		class AudioFormats final
		{
		private:
			std::vector<IAudioFormat*> formats;
			std::vector<std::wstring> SplitExtensions(std::wstring extension);
			bool CompareExtensions(std::wstring lhs, std::wstring rhs);
		public:
			AudioFormats();
			AudioFormats(const AudioFormats&) = delete;
			AudioFormats& operator=(const AudioFormats&) = delete;
			~AudioFormats();
		public:
			void RegisterFormat(IAudioFormat* format);
			IAudioFormat* GetAudioFormat(AudioFile& file);
			IAudioFormat* GetAudioFormat(std::wstring filePath);
		};
	}
}