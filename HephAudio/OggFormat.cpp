#if defined(HEPHAUDIO_USE_FFMPEG)
#include "OggFormat.h"
#include "FFmpegAudioDecoder.h"
#include "../HephCommon/HeaderFiles/HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		StringBuffer OggFormat::Extensions()
		{
			return ".ogg .oga .ogv";
		}
		bool OggFormat::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(0);
			uint32_t data32 = 0;
			audioFile.Read(&data32, 4, Endian::Big);
			return data32 == 0x4F676753;
		}
		size_t OggFormat::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo OggFormat::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormat();
		}
		AudioBuffer OggFormat::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer OggFormat::ReadFile(const HephCommon::File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "OggFormat::ReadFile", "Currently not supported due to the bugs in the FFmpegAudioDecoder class."));
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			const AudioBuffer decodedBuffer = this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
			if (finishedPlaying != nullptr)
			{
				*finishedPlaying = (frameIndex + frameCount) >= this->ffmpegAudioDecoder.GetFrameCount();
			}
			return decodedBuffer;
		}
		bool OggFormat::SaveToFile(const HephCommon::StringBuffer& filePath, AudioBuffer& buffer, bool overwrite)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "OggFormat::SaveToFile", "Not implemented."));
		}
	}
}
#endif