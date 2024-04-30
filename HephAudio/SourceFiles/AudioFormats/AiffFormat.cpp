#include "AudioFormats/AiffFormat.h"
#include "AudioProcessor.h"
#include "AudioCodecs/AudioCodecManager.h"
#include "HephException.h"

#define AIFC_SOWT (HEPHAUDIO_FORMAT_TAG_PCM << 8)
#define AIFF_FORMAT_ID 0x464F524D
#define AIFF_AIFF_ID 0x41494646
#define AIFF_AIFC_ID 0x41494643

using namespace HephCommon;
using namespace HephAudio::Codecs;

namespace HephAudio
{
	namespace FileFormats
	{
		std::string AiffFormat::Extensions()
		{
			return "aif aiff aifc";
		}
		bool AiffFormat::VerifySignature(const File& audioFile)
		{
			uint32_t data32 = 0;

			audioFile.SetOffset(0);
			audioFile.Read(&data32, 4, Endian::Big);
			if (data32 == AIFF_FORMAT_ID)
			{
				audioFile.IncreaseOffset(4);
				audioFile.Read(&data32, 4, Endian::Big);
				return data32 == AIFF_AIFF_ID || data32 == AIFF_AIFC_ID;
			}

			return false;
		}
		size_t AiffFormat::FileFrameCount(const File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo AiffFormat::ReadAudioFormatInfo(const File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
		}
		AudioBuffer AiffFormat::ReadFile(const File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer AiffFormat::ReadFile(const File& audioFile, size_t frameIndex, size_t frameCount)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
		}
		bool AiffFormat::SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite)
		{
			try
			{
				FFmpegAudioEncoder ffmpegAudioEncoder(filePath, buffer.FormatInfo(), overwrite);
				ffmpegAudioEncoder.Encode(buffer);
			}
			catch (HephException)
			{
				return false;
			}

			return true;
		}
	}
}