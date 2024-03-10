#if defined(HEPHAUDIO_USE_FFMPEG)
#include "MpegFormat.h"
#include "FFmpegAudioDecoder.h"
#include "../HephCommon/HeaderFiles/HephException.h"

using namespace HephCommon;

namespace HephAudio
{
	namespace FileFormats
	{
		StringBuffer MpegFormat::Extensions()
		{
			return ".mpeg .mpg";
		}
		bool MpegFormat::VerifySignature(const HephCommon::File& audioFile)
		{
			audioFile.SetOffset(0);
			uint32_t data32 = 0;
			audioFile.Read(&data32, 2, Endian::Big);
			data32 &= 0xFFF0;
			return data32 == 0xFFF0 || data32 == 0xFFE0;
		}
		size_t MpegFormat::FileFrameCount(const HephCommon::File& audioFile, const AudioFormatInfo& audioFormatInfo)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetFrameCount();
		}
		AudioFormatInfo MpegFormat::ReadAudioFormatInfo(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.GetOutputFormat();
		}
		AudioBuffer MpegFormat::ReadFile(const HephCommon::File& audioFile)
		{
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			return this->ffmpegAudioDecoder.Decode();
		}
		AudioBuffer MpegFormat::ReadFile(const HephCommon::File& audioFile, const Codecs::IAudioCodec* pAudioCodec, const AudioFormatInfo& audioFormatInfo, size_t frameIndex, size_t frameCount, bool* finishedPlaying)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "MpegFormat::ReadFile", "Currently not supported."));
			this->ffmpegAudioDecoder.ChangeFile(audioFile.FilePath());
			const AudioBuffer decodedBuffer = this->ffmpegAudioDecoder.Decode(frameIndex, frameCount);
			if (finishedPlaying != nullptr)
			{
				*finishedPlaying = (frameIndex + frameCount) >= this->ffmpegAudioDecoder.GetFrameCount();
			}
			return decodedBuffer;
		}
		bool MpegFormat::SaveToFile(const HephCommon::StringBuffer& filePath, AudioBuffer& buffer, bool overwrite)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_NOT_IMPLEMENTED, "MpegFormat::SaveToFile", "Not implemented."));
		}
	}
}
#endif