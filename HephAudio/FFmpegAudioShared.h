#pragma once
#if defined(HEPHAUDIO_USE_FFMPEG)
#include "HephAudioShared.h"

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
	inline HEPH_CONSTEVAL AVSampleFormat InternalSampleFormat()
	{
#if HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
		return sizeof(heph_audio_sample) == sizeof(double) ? AV_SAMPLE_FMT_DBL : AV_SAMPLE_FMT_FLT;
#else
#error Unsupported internal format??
#endif
	}
}

#define HEPHAUDIO_INTERNAL_SAMPLE_FMT HephAudio::InternalSampleFormat()

#endif

#endif