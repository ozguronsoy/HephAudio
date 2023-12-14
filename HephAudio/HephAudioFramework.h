#pragma once
#include "../HephCommon/HeaderFiles/HephCommonFramework.h"
#include <cmath>

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_QUALCOMM_PUREVOICE 0x0150
#define WAVE_FORMAT_WMAUDIO2 0x0161
#define WAVE_FORMAT_WMAUDIO3 0x0162
#define WAVE_FORMAT_WMAUDIO_LOSSLESS 0x0163
#define WAVE_FORMAT_WMAVOICE9 0x000A
#define WAVE_FORMAT_WMAVOICE10 0x000B
#define WAVE_FORMAT_MPEG 0x0050
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#define WAVE_FORMAT_ALAC 0x6C61
#define WAVE_FORMAT_OPUS 0x704F
#define WAVE_FORMAT_MPEG4_AAC 0xA106
#define WAVE_FORMAT_FLAC 0xF1AC
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

#ifndef HEPHAUDIO_STOPWATCH_START
#ifdef HEPHAUDIO_INFO_LOGGING
#define HEPHAUDIO_STOPWATCH_START HephCommon::StopWatch::StaticStart()
#define HEPHAUDIO_STOPWATCH_RESET HephCommon::StopWatch::StaticReset()
#define HEPHAUDIO_STOPWATCH_DT(prefix) HephCommon::StopWatch::StaticDeltaTime(prefix)
#define HEPHAUDIO_STOPWATCH_STOP HephCommon::StopWatch::StaticStop()
#define HEPHAUDIO_LOG(logMessage, logType) HephCommon::ConsoleLogger::Log(logMessage, logType, "HephAudio")
#else
#define HEPHAUDIO_STOPWATCH_START
#define HEPHAUDIO_STOPWATCH_RESET
#define HEPHAUDIO_STOPWATCH_DT(prefix)
#define HEPHAUDIO_STOPWATCH_STOP
#define HEPHAUDIO_LOG(logMessage, logType)
#endif
#endif

#ifndef HEPHAUDIO_SPEED_OF_SOUND
#define HEPHAUDIO_SPEED_OF_SOUND 343.0 // speed of sound in dry air at 20�C in meters per second
#endif

#pragma region Helper Methods
namespace HephAudio
{
	inline heph_float DecibelToGain(heph_float decibel)
	{
		return pow(10.0, decibel * 0.05);
	}
	inline heph_float GainToDecibel(heph_float gain)
	{
		return gain == 0 ? -120.0 : 20.0 * log10(abs(gain));
	}
	inline HEPH_CONSTEXPR heph_float SemitoneToCent(heph_float semitone)
	{
		return semitone * 100.0;
	}
	inline HEPH_CONSTEXPR heph_float SemitoneToOctave(heph_float semitone)
	{
		return semitone / 12.0;
	}
	inline HEPH_CONSTEXPR heph_float CentToSemitone(heph_float cent)
	{
		return cent / 100.0;
	}
	inline HEPH_CONSTEXPR heph_float CentToOctave(heph_float cent)
	{
		return cent / 1200.0;
	}
	inline HEPH_CONSTEXPR heph_float OctaveToSemitone(heph_float octave)
	{
		return octave * 12.0;
	}
	inline HEPH_CONSTEXPR heph_float OctaveToCent(heph_float octave)
	{
		return octave * 1200.0;
	}
}
#pragma endregion