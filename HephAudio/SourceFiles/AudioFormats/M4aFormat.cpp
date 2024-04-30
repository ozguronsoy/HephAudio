#include "AudioFormats/M4aFormat.h"
#include "HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		std::string M4aFormat::Extensions()
		{
			return "m4a alac";
		}
		bool M4aFormat::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(4);
			uint64_t data64 = 0;
			audioFile.Read(&data64, 8, Endian::Big);
			return data64 == 0x667479704D344120;
		}
		size_t M4aFormat::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo M4aFormat::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
		}
		AudioBuffer M4aFormat::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer M4aFormat::ReadFile(const HephCommon::File& audioFile, size_t frameIndex, size_t frameCount)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
		}
		bool M4aFormat::SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite)
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