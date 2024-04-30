#include "AudioFormats/WavFormat.h"
#include "AudioProcessor.h"
#include "AudioCodecs/AudioCodecManager.h"
#include "HephException.h"

#define WAV_RIFF_ID 0x52494646
#define WAV_WAVE_ID 0x057415645

using namespace HephCommon;
using namespace HephAudio::Codecs;


namespace HephAudio
{
	namespace FileFormats
	{
		std::string WavFormat::Extensions()
		{
			return "wav wave";
		}
		bool WavFormat::VerifySignature(const File& audioFile)
		{
			uint32_t data32 = 0;

			audioFile.SetOffset(0);
			audioFile.Read(&data32, 4, Endian::Big);
			if (data32 == WAV_RIFF_ID)
			{
				audioFile.SetOffset(8);
				audioFile.Read(&data32, 4, Endian::Big);
				return data32 == WAV_WAVE_ID;
			}

			return false;
		}
		size_t WavFormat::FileFrameCount(const File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo WavFormat::ReadAudioFormatInfo(const File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
		}
		AudioBuffer WavFormat::ReadFile(const File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer WavFormat::ReadFile(const File& audioFile, size_t frameIndex, size_t frameCount)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
		}
		bool WavFormat::SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite)
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