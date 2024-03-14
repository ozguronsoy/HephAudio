#pragma once
#include "HephCommonShared.h"
#include "StringBuffer.h"

#define HEPH_CL_INFO "36"
#define HEPH_CL_WARNING "33"
#define HEPH_CL_ERROR "31"
#define HEPH_CL_SUCCESS "32"
#define HEPH_CL_DEBUG "35"

namespace HephCommon
{
	class ConsoleLogger final
	{
	private:
		static bool coloredOutput;
	public:
		ConsoleLogger() = delete;
		ConsoleLogger(const ConsoleLogger&) = delete;
		ConsoleLogger& operator=(const ConsoleLogger&) = delete;
		static void Log(StringBuffer message, const char* logLevel);
		static void Log(StringBuffer message, const char* logLevel, StringBuffer libName);
		static void LogInfo(const StringBuffer& message);
		static void LogInfo(const StringBuffer& message, const StringBuffer& libName);
		static void LogWarning(const StringBuffer& message);
		static void LogWarning(const StringBuffer& message, const StringBuffer& libName);
		static void LogError(const StringBuffer& message);
		static void LogError(const StringBuffer& message, const StringBuffer& libName);
		static void LogSuccess(const StringBuffer& message);
		static void LogSuccess(const StringBuffer& message, const StringBuffer& libName);
		static void LogDebug(const StringBuffer& message);
		static void LogDebug(const StringBuffer& message, const StringBuffer& libName);
		static void EnableColoredOutput();
		static void DisableColoredOutput();
	private:
		static StringBuffer CurrentTimeToString(StringType stringType);
		static StringBuffer GetLogLevelName(const char* logLevel, StringType stringType);
	};
}