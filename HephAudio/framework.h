#pragma once
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

#ifndef PI
#define PI ((HEPHAUDIO_DOUBLE)3.14159265358979323846)
#endif

#ifndef E_FAIL
#define E_FAIL 0x80004005L
#endif

#ifndef E_INVALIDARG
#define E_INVALIDARG 0x80070057L
#endif

#ifndef E_NOTIMPL
#define E_NOTIMPL 0x80004001L
#endif

#ifndef E_OUTOFMEMORY
#define E_OUTOFMEMORY 0x8007000E
#endif

#ifndef WAVE_FORMAT_HEPHAUDIO
#define WAVE_FORMAT_HEPHAUDIO 37
#endif

#ifndef HEPHAUDIO_DOUBLE
#ifndef HEPHAUDIO_HIGH_PRECISION
#define HEPHAUDIO_DOUBLE double
#else
#define HEPHAUDIO_DOUBLE float
#endif
#endif

#ifndef HEPHAUDIO_STOPWATCH_START
#ifdef HEPHAUDIO_INFO_LOGGING
#define HEPHAUDIO_STOPWATCH_START StopWatch::Start()
#define HEPHAUDIO_STOPWATCH_RESET StopWatch::Reset()
#define HEPHAUDIO_STOPWATCH_DT(prefix) StopWatch::DeltaTime(prefix)
#define HEPHAUDIO_STOPWATCH_STOP StopWatch::Stop()
#define HEPHAUDIO_LOG(logMessage, logType) ConsoleLogger::Log(logMessage, logType)
#define HEPHAUDIO_LOG_LINE(logMessage, logType) ConsoleLogger::LogLine(logMessage, logType)
#else
#define HEPHAUDIO_STOPWATCH_START
#define HEPHAUDIO_STOPWATCH_RESET
#define HEPHAUDIO_STOPWATCH_DT(prefix)
#define HEPHAUDIO_STOPWATCH_STOP
#define HEPHAUDIO_LOG(logMessage, logType)
#define HEPHAUDIO_LOG_LINE(logMessage, logType)
#endif
#endif

#pragma region Helper Methods
inline constexpr HEPHAUDIO_DOUBLE sgn(HEPHAUDIO_DOUBLE x)
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
inline HEPHAUDIO_DOUBLE DecibelToGain(HEPHAUDIO_DOUBLE decibel)
{
	return pow(10.0, decibel * 0.05);
}
inline HEPHAUDIO_DOUBLE GainToDecibel(HEPHAUDIO_DOUBLE gain)
{
	return gain == 0 ? -120.0 : 20.0 * log10(abs(gain));
}
inline HEPHAUDIO_DOUBLE RadToDeg(HEPHAUDIO_DOUBLE rad)
{
	return rad * 180.0 / PI;
}
inline HEPHAUDIO_DOUBLE DegToRad(HEPHAUDIO_DOUBLE deg)
{
	return deg * PI / 180.0;
}
#pragma endregion