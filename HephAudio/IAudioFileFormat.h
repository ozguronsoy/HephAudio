#pragma once
#include "framework.h"
#include "AudioBuffer.h"
#include "AudioFile.h"
#include "AudioFormatInfo.h"
#include "StringBuffer.h"
#include <memory>

namespace HephAudio
{
	namespace FileFormats
	{
		constexpr Endian operator!(const Endian& lhs) { return lhs == Endian::Big ? Endian::Little : Endian::Big; }
		class IAudioFileFormat
		{
		public:
			virtual ~IAudioFileFormat() = default;
			// Gets the extension of the current format.
			virtual StringBuffer Extension() const = 0;
			// Reads the audio format information.
			virtual AudioFormatInfo ReadAudioFormatInfo(const AudioFile* pAudioFile) const = 0;
			// Reads audio data from the given file and converts it to a pcm buffer.
			// File format must be the current format or this method will return an empty buffer.
			virtual AudioBuffer ReadFile(const AudioFile* pAudioFile) const = 0;
			// Converts pcm buffer to current format and then writes it into a file.
			virtual bool SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const = 0;
		};
	}
}