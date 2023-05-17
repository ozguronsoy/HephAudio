#include "AudioFileFormatManager.h"
#include "WavFormat.h"
#include "AiffFormat.h"

namespace HephAudio
{
	namespace FileFormats
	{
		std::vector<IAudioFileFormat*> formats = {
					new WavFormat(),
					new AiffFormat()
		};

		bool AudioFileFormatManager::CompareExtensions(HephCommon::StringBuffer lhs, HephCommon::StringBuffer rhs)
		{
			std::vector<HephCommon::StringBuffer> lhse = lhs.Split(' ');
			std::vector<HephCommon::StringBuffer> rhse = rhs.Split(' ');
			for (size_t i = 0; i < lhse.size(); i++)
			{
				for (size_t j = 0; j < rhse.size(); j++)
				{
					if (lhse.at(i).CompareContent(rhse.at(j)))
					{
						return true;
					}
				}
			}
			return false;
		}
		void AudioFileFormatManager::RegisterFileFormat(IAudioFileFormat* format)
		{
			for (size_t i = 0; i < formats.size(); i++)
			{
				// If format is already registered, remove it and add the new format.
				// This is for when a user wants to implement his/her own format.
				if (CompareExtensions(format->Extension(), formats.at(i)->Extension()))
				{
					delete formats.at(i);
					formats.erase(formats.begin() + i);
					break;
				}
			}
			formats.push_back(format);
		}
		IAudioFileFormat* AudioFileFormatManager::FindFileFormat(HephCommon::File& file)
		{
			for (size_t i = 0; i < formats.size(); i++)
			{
				if (formats.at(i)->Extension().Contains(file.FileExtension()))
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
				if (formats.at(i)->Extension().Contains(fileExtension))
				{
					return formats.at(i);
				}
			}
			return nullptr;
		}
	}
}