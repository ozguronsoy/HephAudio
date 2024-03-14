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

	inline HEPH_CONSTEVAL AVSampleFormat InternalSampleFormat()
	{
#if HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
		return sizeof(heph_audio_sample) == sizeof(double) ? AV_SAMPLE_FMT_DBL : AV_SAMPLE_FMT_FLT;
#else
#error Unsupported internal format??
#endif
	}
}

#define HEPHAUDIO_FFMPEG_GET_ERROR_MESSAGE(errorCode) HephAudio::FFmpegGetErrorMessage(errorCode)
#define HEPHAUDIO_FFMPEG_INTERNAL_SAMPLE_FMT HephAudio::InternalSampleFormat()

#endif

#endif