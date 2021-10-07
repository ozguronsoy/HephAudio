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
			std::vector<std::wstring> SplitExtensions(std::wstring extension)
			{
				std::vector<std::wstring> splitExtensions;
				uint32_t cursor = 0;
				while (true)
				{
					const size_t pos = extension.find(' ', cursor);
					splitExtensions.push_back(extension.substr(cursor, pos - cursor));
					cursor = pos + 1;
					if (pos == std::wstring::npos)
					{
						break;
					}
				}
				return splitExtensions;
			}
			bool CompareExtensions(std::wstring lhs, std::wstring rhs)
			{
				std::vector<std::wstring> lhse = SplitExtensions(lhs);
				std::vector<std::wstring> rhse = SplitExtensions(rhs);
				for (size_t i = 0; i < lhse.size(); i++)
				{
					for (size_t j = 0; j < rhse.size(); j++)
					{
						if (lhse.at(i) == rhse.at(j))
						{
							return true;
						}
					}
				}
				return false;
			}
		public:
			AudioFormats()
			{
				formats.push_back(new WavFormat());
				formats.push_back(new AiffFormat());
				formats.push_back(new Mp3Format());
			}
			AudioFormats(const AudioFormats&) = delete;
			AudioFormats& operator=(const AudioFormats&) = delete;
			~AudioFormats()
			{
				for (size_t i = 0; i < formats.size(); i++)
				{
					delete formats.at(i);
				}
			}
		public:
			void RegisterFormat(IAudioFormat* format)
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
			IAudioFormat* GetAudioFormat(AudioFile& file)
			{
				for (size_t i = 0; i < formats.size(); i++)
				{
					if (CompareExtensions(file.Extension(), formats.at(i)->Extension()))
					{
						return formats.at(i);
					}
				}
				return nullptr;
			}
			IAudioFormat* GetAudioFormat(std::wstring filePath)
			{
				for (size_t i = 0; i < formats.size(); i++)
				{
					if (CompareExtensions(AudioFile::GetFileExtension(filePath), formats.at(i)->Extension()))
					{
						return formats.at(i);
					}
				}
				return nullptr;
			}
		};
	}
}