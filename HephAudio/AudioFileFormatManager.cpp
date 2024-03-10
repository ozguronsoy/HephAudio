#include "AudioFileFormatManager.h"
#include "WavFormat.h"
#include "AiffFormat.h"
#include "Mp3Format.h"
#include "MpegFormat.h"
#include "OggFormat.h"
#include "FlacFormat.h"
#include "M4aFormat.h"
#include "AacFormat.h"
#include "WmaFormat.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		std::vector<IAudioFileFormat*> formats = {
			new WavFormat(),
			new AiffFormat(),
#if defined(HEPHAUDIO_USE_FFMPEG)
			new Mp3Format(),
			new MpegFormat(),
			new OggFormat(),
			new FlacFormat(),
			new M4aFormat(),
			new AacFormat(),
			new WmaFormat(),
#endif
		};

		void AudioFileFormatManager::RegisterFileFormat(IAudioFileFormat* format)
		{
			const std::vector<StringBuffer> newFormatExtensions = format->Extensions().Split(' ');
			for (size_t i = 0; i < formats.size(); i++)
			{
				const StringBuffer currentFormatExtensions = formats.at(i)->Extensions();
				for (size_t j = 0; j < newFormatExtensions.size(); j++)
				{
					// If format is already registered, remove it and add the new implementation.
					if (currentFormatExtensions.Contains(newFormatExtensions[j]))
					{
						delete formats.at(i);
						formats.erase(formats.begin() + i);
						formats.push_back(format);
						return;
					}
				}
			}
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
		IAudioFileFormat* AudioFileFormatManager::FindFileFormat(StringBuffer filePath)
		{
			const StringBuffer fileExtension = File::GetFileExtension(filePath);
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