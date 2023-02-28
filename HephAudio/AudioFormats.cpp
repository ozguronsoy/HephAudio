#include "AudioFormats.h"

namespace HephAudio
{
	namespace Formats
	{
		bool AudioFormats::CompareExtensions(StringBuffer lhs, StringBuffer rhs)
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
		AudioFormats::AudioFormats()
		{
			formats =
			{
				new WavFormat(),
				new AiffFormat()
			};
		}
		AudioFormats::~AudioFormats()
		{
			for (size_t i = 0; i < formats.size(); i++)
			{
				delete formats.at(i);
			}
		}
		void AudioFormats::RegisterFormat(IAudioFormat* format)
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
		IAudioFormat* AudioFormats::GetAudioFormat(AudioFile& file)
		{
			for (size_t i = 0; i < formats.size(); i++)
			{
				if (formats.at(i)->Extension().Contains(file.Extension()))
				{
					return formats.at(i);
				}
			}
			return nullptr;
		}
		IAudioFormat* AudioFormats::GetAudioFormat(StringBuffer filePath)
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