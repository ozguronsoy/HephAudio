#pragma once
#if defined(HEPHAUDIO_USE_FFMPEG)
#include "HephAudioShared.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")



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
	inline HephCommon::StringBuffer FFmpegGetErrorMessage(int errorCode)
	{
		char errorMessage[AV_ERROR_MAX_STRING_SIZE]{ };
		if (av_strerror(errorCode, errorMessage, AV_ERROR_MAX_STRING_SIZE) < 0)
		{
			return "error message not found.";
		}
		return errorMessage;
	}
}

#define HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(errorCode) HephAudio::FFmpegGetErrorMessage(errorCode)


#if defined(HEPHAUDIO_SAMPLE_TYPE_DBL)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_DBL
#elif defined(HEPHAUDIO_SAMPLE_TYPE_FLT)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_FLT
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S64)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S64
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S32)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S32
#elif defined(HEPHAUDIO_SAMPLE_TYPE_S16)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_S16
#else
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT AV_SAMPLE_FMT_FLT
#endif

#endif

#endif