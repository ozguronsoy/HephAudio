#include "AudioFormats/AudioFileFormatManager.h"
#include "AudioFormats/WavFormat.h"
#include "AudioFormats/AiffFormat.h"
#include "AudioFormats/MpegFormat.h"
#include "AudioFormats/Mp3Format.h"
#include "AudioFormats/Mp4Format.h"
#include "AudioFormats/OggFormat.h"
#include "AudioFormats/FlacFormat.h"
#include "AudioFormats/M4aFormat.h"
#include "AudioFormats/AacFormat.h"
#include "AudioFormats/WmaFormat.h"
#include "StringHelpers.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		std::vector<IAudioFileFormat*> formats = {
			new WavFormat(),
			new AiffFormat(),
#if defined(HEPHAUDIO_USE_FFMPEG)
			new MpegFormat(),
			new Mp3Format(),
			new Mp4Format(),
			new OggFormat(),
			new FlacFormat(),
			new M4aFormat(),
			new AacFormat(),
			new WmaFormat(),
#endif
		};

		void AudioFileFormatManager::RegisterFileFormat(IAudioFileFormat* format)
		{
			const std::vector<std::string> newFormatExtensions = StringHelpers::Split(format->Extensions(), " ");
			for (size_t i = 0; i < formats.size(); i++)
			{
				const std::string currentFormatExtensions = formats.at(i)->Extensions();
				for (size_t j = 0; j < newFormatExtensions.size(); j++)
				{
					// If format is already registered, remove it and add the new implementation.
					if (currentFormatExtensions.find_first_of(newFormatExtensions[j]) != std::string::npos)
					{
						delete formats.at(i);
						formats.erase(formats.begin() + i);
						formats.push_back(format);
						return;
					}
				}
			}
			formats.push_back(format);
		}
		IAudioFileFormat* AudioFileFormatManager::FindFileFormat(const File& file)
		{
			for (size_t i = 0; i < formats.size(); i++)
			{
				if (formats.at(i)->VerifySignature(file))
				{
					return formats.at(i);
				}
			}
			return nullptr;
		}
		IAudioFileFormat* AudioFileFormatManager::FindFileFormat(const std::string& filePath)
		{
			const std::string fileExtension = File::GetFileExtension(filePath);
			for (size_t i = 0; i < formats.size(); i++)
			{
				if (formats.at(i)->VerifyExtension(fileExtension))
				{
					return formats.at(i);
				}
			}
			return nullptr;
		}
	}
}