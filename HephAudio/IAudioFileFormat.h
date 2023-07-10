#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "File.h"
#include "AudioFormatInfo.h"
#include "StringBuffer.h"
#include "IAudioCodec.h"
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
			virtual HephCommon::StringBuffer Extension() const = 0;
			// Gets the number of frames the file consists of when decoded.
			virtual size_t FileFrameCount(const HephCommon::File* pAudioFile, const AudioFormatInfo& audioFormatInfo) const = 0;
			// Reads the audio format information.
			virtual AudioFormatInfo ReadAudioFormatInfo(const HephCommon::File* pAudioFile) const = 0;
			// Reads audio data from the given file and converts it to a pcm buffer.
			virtual AudioBuffer ReadFile(const HephCommon::File* pAudioFile) const = 0;
			// Reads the desired portion of the audio data from the given file and converts it to a pcm buffer.
			virtual AudioBuffer ReadFile(const HephCommon::File* pAudioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying) const = 0;
			// Converts pcm buffer to current format and then writes it into a file.
			virtual bool SaveToFile(HephCommon::StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const = 0;
		};
	}
}