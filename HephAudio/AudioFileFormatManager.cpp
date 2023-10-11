#include "AudioFileFormatManager.h"
#include "WavFormat.h"
#include "AiffFormat.h"
#include "WmaFormat.h"
#include "MpegFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		std::vector<IAudioFileFormat*> formats = {
					new WavFormat(),
					new AiffFormat(),
					new WmaFormat(),
					new MpegFormat()
		};

		void AudioFileFormatManager::RegisterFileFormat(IAudioFileFormat* format)
		{
			for (size_t i = 0; i < formats.size(); i++)
			{
				// If format is already registered, remove it and add the new implementation.
				if (format->Extensions().Contains(formats.at(i)->Extensions()))
				{
					delete formats.at(i);
					formats.erase(formats.begin() + i);
					break;
				}
			}
			formats.push_back(format);
		}
		IAudioFileFormat* AudioFileFormatManager::FindFileFormat(const HephCommon::File& file)
		{
			for (size_t i = 0; i < formats.size(); i++)
			{
				if (formats.at(i)->CheckSignature(file))
				{
					return formats.at(i);
				}
			}
			return nullptr;
		}
		IAudioFileFormat* AudioFileFormatManager::FindFileFormat(HephCommon::StringBuffer filePath)
		{
			const HephCommon::StringBuffer fileExtension = HephCommon::File::GetFileExtension(filePath);
			for (size_t i = 0; i < formats.size(); i++)
			{
				if (formats.at(i)->Extensions().Contains(fileExtension))
				{
					return formats.at(i);
				}
			}
			return nullptr;
		}
	}
}