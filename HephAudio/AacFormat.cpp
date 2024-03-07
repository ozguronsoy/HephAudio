#if defined(HEPHAUDIO_USE_FFMPEG)
#include "AacFormat.h"
#include "FFmpegAudioDecoder.h"
#include "../HephCommon/HeaderFiles/HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		StringBuffer AacFormat::Extensions()
		{
			return ".m4a .aac .alac";
		}
		bool AacFormat::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(4);
			uint64_t data64 = 0;
			audioFile.Read(&data64, 8, Endian::Big);
			return data64 == 0x667479704D344120;
		}
		size_t AacFormat::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo AacFormat::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormat();
		}
		AudioBuffer AacFormat::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer AacFormat::ReadFile(const HephCommon::File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AacFormat::ReadFile", "Currently not supported due to the bugs in the FFmpegAudioDecoder class."));
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			const AudioBuffer decodedBuffer = this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
			if (finishedPlaying != nullptr)
			{
				*finishedPlaying = (frameIndex + frameCount) >= this->ffmpegAudioDecoder.GetFrameCount();
			}
			return decodedBuffer;
		}
		bool AacFormat::SaveToFile(const HephCommon::StringBuffer& filePath, AudioBuffer& buffer, bool overwrite)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "AacFormat::SaveToFile", "Not implemented."));
		}
	}
}
#endif