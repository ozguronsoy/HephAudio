#include "AudioFormats/AacFormat.h"
#include "HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		std::string AacFormat::Extensions()
		{
			return "aac adts";
		}
		bool AacFormat::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(0);
			uint16_t data16 = 0;
			audioFile.Read(&data16, 2, Endian::Big);
			return data16 == 0xFFF1 || data16 == 0xFFF9;
		}
		size_t AacFormat::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo AacFormat::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
		}
		AudioBuffer AacFormat::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer AacFormat::ReadFile(const HephCommon::File& audioFile, size_t frameIndex, size_t frameCount)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
		}
		bool AacFormat::SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite)
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