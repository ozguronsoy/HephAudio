#include "AudioFormats/FlacFormat.h"
#include "HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		std::string FlacFormat::Extensions()
		{
			return "flac";
		}
		bool FlacFormat::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(0);
			uint32_t data32 = 0;
			audioFile.Read(&data32, 4, Endian::Big);
			return data32 == 0x664C6143;
		}
		size_t FlacFormat::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo FlacFormat::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
		}
		AudioBuffer FlacFormat::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer FlacFormat::ReadFile(const HephCommon::File& audioFile, size_t frameIndex, size_t frameCount)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
		}
		bool FlacFormat::SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite)
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