#include "AudioFormatManager.h"
#include "WavFormat.h"
#include "AiffFormat.h"

namespace HephAudio
{
	namespace Formats
	{
		std::vector<IAudioFormat*> formats = {
					new WavFormat(),
					//new AiffFormat()
		};

		bool AudioFormatManager::CompareExtensions(StringBuffer lhs, StringBuffer rhs)
		{
			std::vector<StringBuffer> lhse = lhs.Split(' ');
			std::vector<StringBuffer> rhse = rhs.Split(' ');
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
		void AudioFormatManager::RegisterFormat(IAudioFormat* format)
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
		IAudioFormat* AudioFormatManager::FindAudioFormat(AudioFile& file)
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
		IAudioFormat* AudioFormatManager::FindAudioFormat(StringBuffer filePath)
		{
			const StringBuffer fileExtension = AudioFile::GetFileExtension(filePath);
			for (size_t i = 0; i < formats.size(); i++)
			{
				if (formats.at(i)->Extension().Contains(fileExtension));
				{
					return formats.at(i);
				}
			}
			return nullptr;
		}
	}
}