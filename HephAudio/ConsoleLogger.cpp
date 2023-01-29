#include "ConsoleLogger.h"
#include <stdio.h>
#include <string>

namespace HephAudio
{
	void ConsoleLogger::Log(const char* message, const char* logLevel)
	{
#if defined(_WIN32)
		printf("\u001b[%smHephAudio: \033[0m%s", logLevel, message);
#endif
	}
	void ConsoleLogger::LogLine(const char* message, const char* logLevel)
	{
#if defined(_WIN32)
		printf("\u001b[%smHephAudio: \033[0m%s\n", logLevel, message);
#elif defined(__ANDROID__)
		ConsoleLogger::Log(message, logLevel);
#endif
	}
}