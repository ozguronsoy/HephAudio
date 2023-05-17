#pragma once
#include "HephCommonFramework.h"
#include "StringBuffer.h"

namespace HephCommon
{
	class ConsoleLogger final
	{
	public:
		static constexpr const char* info = "36";
		static constexpr const char* warning = "33";
		static constexpr const char* error = "31";
		static constexpr const char* success = "32";
		static constexpr const char* debug = "35";
	public:
		ConsoleLogger() = delete;
		ConsoleLogger(const ConsoleLogger&) = delete;
		ConsoleLogger& operator=(const ConsoleLogger&) = delete;
		static void Log(StringBuffer message, const char* logLevel);
		static void Log(StringBuffer message, const char* logLevel, StringBuffer libName);
	private:
		static StringBuffer CurrentTimeToString(StringType stringType);
	};
}