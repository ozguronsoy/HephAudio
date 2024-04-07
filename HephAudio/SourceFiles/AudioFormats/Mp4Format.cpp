#if defined(HEPHAUDIO_USE_FFMPEG)
#include "AudioFormats/Mp4Format.h"
#include "HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		std::string Mp4Format::Extensions()
		{
			return "mp4 m4a m4p m4b m4r m4v";
		}
		bool Mp4Format::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(4);
			uint64_t data64 = 0;
			audioFile.Read(&data64, 8, Endian::Big);

			return data64 == 0x667479704D534E56 || data64 == 0x6674797069736F6D || data64 == 0x667479706D703432;
		}
		size_t Mp4Format::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo Mp4Format::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormatInfo();
		}
		AudioBuffer Mp4Format::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer Mp4Format::ReadFile(const HephCommon::File& audioFile, Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			const AudioBuffer decodedBuffer = this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
			if (finishedPlaying != nullptr)
			{
				*finishedPlaying = (frameIndex + frameCount) >= this->ffmpegAudioDecoder.GetFrameCount();
			}
			return decodedBuffer;
		}
		bool Mp4Format::SaveToFile(const std::string& filePath, AudioBuffer& buffer, bool overwrite)
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