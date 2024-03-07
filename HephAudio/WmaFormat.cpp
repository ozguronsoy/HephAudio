#if defined(HEPHAUDIO_USE_FFMPEG)
#include "WmaFormat.h"
#include "FFmpegAudioDecoder.h"
#include "../HephCommon/HeaderFiles/HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		StringBuffer WmaFormat::Extensions()
		{
			return ".wma .asf .wmv";
		}
		bool WmaFormat::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(0);
			uint64_t data64_1 = 0, data64_2 = 0;
			audioFile.Read(&data64_1, 8, Endian::Big);
			audioFile.Read(&data64_2, 8, Endian::Big);
			return data64_1 == 0x3026B2758E66CF11 && data64_2 == 0xA6D900AA0062CE6C;
		}
		size_t WmaFormat::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo WmaFormat::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormat();
		}
		AudioBuffer WmaFormat::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer WmaFormat::ReadFile(const HephCommon::File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "WmaFormat::ReadFile", "Currently not supported due to the bugs in the FFmpegAudioDecoder class."));
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			const AudioBuffer decodedBuffer = this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
			if (finishedPlaying != nullptr)
			{
				*finishedPlaying = (frameIndex + frameCount) >= this->ffmpegAudioDecoder.GetFrameCount();
			}
			return decodedBuffer;
		}
		bool WmaFormat::SaveToFile(const HephCommon::StringBuffer& filePath, AudioBuffer& buffer, bool overwrite)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "WmaFormat::SaveToFile", "Not implemented."));
		}
	}
}
#endif