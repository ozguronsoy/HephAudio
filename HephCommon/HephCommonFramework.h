#pragma once

#ifndef PI
#define PI (3.14159265358979323846)
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

#ifndef HEPHAUDIO_STOPWATCH_START
#ifdef HEPHAUDIO_INFO_LOGGING
#define HEPHAUDIO_STOPWATCH_START HephCommon::StopWatch::Start()
#define HEPHAUDIO_STOPWATCH_RESET HephCommon::StopWatch::Reset()
#define HEPHAUDIO_STOPWATCH_DT(prefix) HephCommon::StopWatch::DeltaTime(prefix)
#define HEPHAUDIO_STOPWATCH_STOP HephCommon::StopWatch::Stop()
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

enum class Endian : unsigned char
{
	Little = 0,
	Big = 1
};