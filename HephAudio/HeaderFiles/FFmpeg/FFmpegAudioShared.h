#pragma once
#include "HephAudioShared.h"
#include "AudioFormatInfo.h"
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#if defined(_WIN32) && defined(_MSVC_LANG)
#pragma comment(lib, "ffmpeg/lib/windows/avcodec.lib")
#pragma comment(lib, "ffmpeg/lib/windows/avformat.lib")
#pragma comment(lib, "ffmpeg/lib/windows/avutil.lib")
#pragma comment(lib, "ffmpeg/lib/windows/swresample.lib")
#endif


#if LIBAVFORMAT_VERSION_MAJOR < 60
#error unsupported libavformat version.
#endif

#if LIBAVCODEC_VERSION_MAJOR < 60
#error unsupported libavcodec version.
#endif

#if LIBAVUTIL_VERSION_MAJOR < 58
#error unsupported libavutil version.
#endif

#if LIBSWRESAMPLE_VERSION_MAJOR < 4
#error unsupported libswresample version.
#endif



#if !defined(HEPHAUDIO_INTERNAL_SAMPLE_FMT)

namespace HephAudio
{
	inline std::string FFmpegGetErrorMessage(int errorCode)
	{
		char errorMessage[AV_ERROR_MAX_STRING_SIZE]{ };
		if (av_strerror(errorCode, errorMessage, AV_ERROR_MAX_STRING_SIZE) < 0)
		{
			return "error message not found.";
		}
		return errorMessage;
	}

	inline AVCodecID CodecIdFromAudioFormatInfo(const AudioFormatInfo& audioFormatInfo)
	{
		switch (audioFormatInfo.formatTag)
		{

		case HEPHAUDIO_FORMAT_TAG_PCM:
		{
			if (audioFormatInfo.endian == HephCommon::Endian::Little)
			{
				switch (audioFormatInfo.bitsPerSample)
				{
				case 8:
					return AV_CODEC_ID_PCM_U8;
				case 16:
					return AV_CODEC_ID_PCM_S16LE;
				case 24:
					return AV_CODEC_ID_PCM_S24LE;
				case 32:
					return AV_CODEC_ID_PCM_S32LE;
				case 64:
					return AV_CODEC_ID_PCM_S64LE;
				default:
					return AV_CODEC_ID_NONE;
				}
			}
			else
			{
				switch (audioFormatInfo.bitsPerSample)
				{
				case 8:
					return AV_CODEC_ID_PCM_U8;
				case 16:
					return AV_CODEC_ID_PCM_S16BE;
				case 24:
					return AV_CODEC_ID_PCM_S24BE;
				case 32:
					return AV_CODEC_ID_PCM_S32BE;
				case 64:
					return AV_CODEC_ID_PCM_S64BE;
				default:
					return AV_CODEC_ID_NONE;
				}
			}
		}

		case HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT:
		{
			if (audioFormatInfo.endian == HephCommon::Endian::Little)
			{
				return audioFormatInfo.bitsPerSample == sizeof(double) ? AV_CODEC_ID_PCM_F64LE : AV_CODEC_ID_PCM_F32LE;
			}
			return audioFormatInfo.bitsPerSample == sizeof(double) ? AV_CODEC_ID_PCM_F64BE : AV_CODEC_ID_PCM_F32BE;
		}

		case HEPHAUDIO_FORMAT_TAG_ALAW:
			return AV_CODEC_ID_PCM_ALAW;

		case HEPHAUDIO_FORMAT_TAG_MULAW:
			return AV_CODEC_ID_PCM_MULAW;

		case HEPHAUDIO_FORMAT_TAG_WMA:
			return AV_CODEC_ID_WMAV2;

		case HEPHAUDIO_FORMAT_TAG_WMAUDIO_LOSSLESS:
			return AV_CODEC_ID_WMALOSSLESS;

		case HEPHAUDIO_FORMAT_TAG_WMAVOICE:
			return AV_CODEC_ID_WMAVOICE;

		case HEPHAUDIO_FORMAT_TAG_MPEG:
			return AV_CODEC_ID_MP2;

		case HEPHAUDIO_FORMAT_TAG_MP3:
			return AV_CODEC_ID_MP3;

		case HEPHAUDIO_FORMAT_TAG_AAC:
			return AV_CODEC_ID_AAC;

		case HEPHAUDIO_FORMAT_TAG_ALAC:
			return AV_CODEC_ID_ALAC;

		case HEPHAUDIO_FORMAT_TAG_FLAC:
			return AV_CODEC_ID_FLAC;

		case HEPHAUDIO_FORMAT_TAG_OPUS:
			return AV_CODEC_ID_OPUS;

		case HEPHAUDIO_FORMAT_TAG_VORBIS:
			return AV_CODEC_ID_VORBIS;

		default:
			return AV_CODEC_ID_NONE;
		}
	}

	inline AVSampleFormat ToAVSampleFormat(const AVCodec* avCodec, const AudioFormatInfo& formatInfo)
	{
		if (avCodec == nullptr || avCodec->sample_fmts == nullptr)
		{
			return AV_SAMPLE_FMT_NONE;
		}

		auto getFirstSupportedSampleFormat = [avCodec](std::vector<AVSampleFormat> possibleFormats) -> AVSampleFormat
			{
				for (size_t i = 0; i < possibleFormats.size(); ++i)
				{
					for (size_t j = 0; avCodec->sample_fmts[j] != AV_SAMPLE_FMT_NONE; ++j)
					{
						if (possibleFormats[i] == avCodec->sample_fmts[j])
						{
							return possibleFormats[i];
						}
					}
				}
				return AV_SAMPLE_FMT_NONE;
			};

		if (formatInfo.formatTag == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT)
		{
			if (formatInfo.bitsPerSample == sizeof(double))
			{
				return getFirstSupportedSampleFormat(
					{
						AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
						AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
						AV_SAMPLE_FMT_S64, AV_SAMPLE_FMT_S64P,
						AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
						AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P
					});
			}
			return getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
					AV_SAMPLE_FMT_S64, AV_SAMPLE_FMT_S64P,
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P
				});
		}

		switch (formatInfo.bitsPerSample)
		{
		case 8:
			return getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P,
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_S64, AV_SAMPLE_FMT_S64P,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP
				});
		case 16:
			return getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_S64, AV_SAMPLE_FMT_S64P,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P
				});
		case 24:
		case 32:
			return getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_S64, AV_SAMPLE_FMT_S64P,
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P
				});
		case 64:
			return getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_S64, AV_SAMPLE_FMT_S64P,
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P
				});
		default:
			return getFirstSupportedSampleFormat(
				{
					AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_DBLP,
					AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLTP,
					AV_SAMPLE_FMT_S64, AV_SAMPLE_FMT_S64P,
					AV_SAMPLE_FMT_S32, AV_SAMPLE_FMT_S32P,
					AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P,
					AV_SAMPLE_FMT_U8, AV_SAMPLE_FMT_U8P
				});
		}
	}

	inline AVSampleFormat ToAVSampleFormat(const AudioFormatInfo& formatInfo)
	{
		const AVCodecID codecID = HephAudio::CodecIdFromAudioFormatInfo(formatInfo);
		const AVCodec* avCodec = avcodec_find_encoder(codecID);
		return HephAudio::ToAVSampleFormat(avCodec, formatInfo);
	}

	inline AVChannelLayout ToAVChannelLayout(const AudioChannelLayout& audioChannelLayout)
	{
		AVChannelLayout avChannelLayout{};
		avChannelLayout.order = AV_CHANNEL_ORDER_NATIVE;
		avChannelLayout.nb_channels = audioChannelLayout.count;
		avChannelLayout.u.mask = (uint64_t)audioChannelLayout.mask;
		return avChannelLayout;
	}

	inline AudioChannelLayout FromAVChannelLayout(const AVChannelLayout& avChannelLayout)
	{
		AudioChannelLayout audioChannelLayout;

		audioChannelLayout.count = avChannelLayout.nb_channels;

		uint32_t avChMask = (uint32_t)avChannelLayout.u.mask;
		audioChannelLayout.mask = (AudioChannelMask)avChMask;

		// some formats like WAV and AIFF do not set mono and stereo masks
		if (audioChannelLayout.mask == AudioChannelMask::Unknown)
		{
			switch (audioChannelLayout.count)
			{
			case 1:
				audioChannelLayout.mask = HEPHAUDIO_CH_MASK_MONO;
				break;
			case 2:
				audioChannelLayout.mask = HEPHAUDIO_CH_MASK_STEREO;
				break;
			default:
				break;
			}
		}

		return audioChannelLayout;
	}

	inline uint32_t GetClosestSupportedSampleRate(const AVCodec* avCodec, uint32_t targetSampleRate)
	{
		if (avCodec->supported_samplerates != nullptr)
		{
			int32_t closestSampleRate = avCodec->supported_samplerates[0];
			int32_t closestAbsDeltaSampleRate = abs((int)targetSampleRate - avCodec->supported_samplerates[0]);
			for (size_t i = 1; avCodec->supported_samplerates[i] != 0; i++)
			{
				if (avCodec->supported_samplerates[i] == targetSampleRate)
				{
					return targetSampleRate;
				}

				const int32_t currentAbsDeltaSampleRate = abs((int)targetSampleRate - avCodec->supported_samplerates[i]);
				if (currentAbsDeltaSampleRate < closestAbsDeltaSampleRate)
				{
					closestAbsDeltaSampleRate = currentAbsDeltaSampleRate;
					closestSampleRate = avCodec->supported_samplerates[i];
				}
				else if (currentAbsDeltaSampleRate == closestAbsDeltaSampleRate && avCodec->supported_samplerates[i] > closestSampleRate) // choose the greater sample rate
				{
					closestSampleRate = avCodec->supported_samplerates[i];
				}
			}
			return closestSampleRate;
		}

		return targetSampleRate;
	}

	inline uint32_t GetClosestSupportedSampleRate(const AudioFormatInfo& formatInfo)
	{
		const AVCodecID codecID = HephAudio::CodecIdFromAudioFormatInfo(formatInfo);
		const AVCodec* avCodec = avcodec_find_encoder(codecID);
		return HephAudio::GetClosestSupportedSampleRate(avCodec, formatInfo.sampleRate);
	}
}

#define HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(errorCode) HephAudio::FFmpegGetErrorMessage(errorCode)


#if defined(HEPH_AUDIO_SAMPLE_TYPE_DBL)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_DBL
#elif defined(HEPH_AUDIO_SAMPLE_TYPE_FLT)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_FLT
#elif defined(HEPH_AUDIO_SAMPLE_TYPE_S64)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S64
#elif defined(HEPH_AUDIO_SAMPLE_TYPE_S32)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S32
#elif defined(HEPH_AUDIO_SAMPLE_TYPE_S16)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S16
#else
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_FLT
#endif

#endif