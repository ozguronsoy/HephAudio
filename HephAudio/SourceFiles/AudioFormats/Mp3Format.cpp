#if defined(HEPHAUDIO_USE_FFMPEG)
#include "AudioFormats/Mp3Format.h"
#include "HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		std::string Mp3Format::Extensions()
		{
			return "mp3";
		}
		bool Mp3Format::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(0);
			uint32_t data32 = 0;
			audioFile.Read(&data32, 3, Endian::Big);
			return data32 == 0xFFFB || data32 == 0xFFF3 || data32 == 0xFFF2 || data32 == 0x494433;
		}
		size_t Mp3Format::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo Mp3Format::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
		}
		AudioBuffer Mp3Format::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer Mp3Format::ReadFile(const HephCommon::File& audioFile, Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			const AudioBuffer decodedBuffer = this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
			if (finishedPlaying != nullptr)
			{
				*finishedPlaying = (frameIndex + frameCount) >= this->ffmpegAudioDecoder.GetFrameCount();
			}
			return decodedBuffer;
		}
		bool Mp3Format::SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite) 
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
#endif