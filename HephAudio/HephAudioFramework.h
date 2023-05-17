#pragma once
#include "HephCommonFramework.h"
#include <cmath>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#endif

#ifdef __ANDROID__
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) > (b)) ? (b) : (a))
#define WAVE_FORMAT_PCM 1
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

#ifndef HEPHAUDIO_STOPWATCH_START
#ifdef HEPHAUDIO_INFO_LOGGING
#define HEPHAUDIO_STOPWATCH_START HephCommon::StopWatch::StaticStart()
#define HEPHAUDIO_STOPWATCH_RESET HephCommon::StopWatch::StaticReset()
#define HEPHAUDIO_STOPWATCH_DT(prefix) HephCommon::StopWatch::StaticDeltaTime(prefix)
#define HEPHAUDIO_STOPWATCH_STOP HephCommon::StopWatch::StaticStop()
#define HEPHAUDIO_LOG(logMessage, logType) HephCommon::ConsoleLogger::Log(logMessage, logType)
#else
#define HEPHAUDIO_STOPWATCH_START
#define HEPHAUDIO_STOPWATCH_RESET
#define HEPHAUDIO_STOPWATCH_DT(prefix)
#define HEPHAUDIO_STOPWATCH_STOP
#define HEPHAUDIO_LOG(logMessage, logType)
#define HEPHAUDIO_LOG(logMessage, logType)
#endif
#endif

#pragma region Helper Methods
inline constexpr heph_float sgn(heph_float x)
{
	if (x > 0)
	{
		return 1.0;
	}
	else if (x < 0)
	{
		return -1.0;
	}
	return 0.0;
}
inline heph_float DecibelToGain(heph_float decibel)
{
	return pow(10.0, decibel * 0.05);
}
inline heph_float GainToDecibel(heph_float gain)
{
	return gain == 0 ? -120.0 : 20.0 * log10(abs(gain));
}
inline constexpr heph_float SemitoneToCent(heph_float semitone)
{
	return semitone * 100.0;
}
inline constexpr heph_float SemitoneToOctave(heph_float semitone)
{
	return semitone / 12.0;
}
inline constexpr heph_float CentToSemitone(heph_float cent)
{
	return cent / 100.0;
}
inline constexpr heph_float CentToOctave(heph_float cent)
{
	return cent / 1200.0;
}
inline constexpr heph_float OctaveToSemitone(heph_float octave)
{
	return octave * 12.0;
}
inline constexpr heph_float OctaveToCent(heph_float octave)
{
	return octave * 1200.0;
}
#pragma endregion