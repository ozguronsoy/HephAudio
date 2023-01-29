#include "ConsoleLogger.h"
#include <stdio.h>
#include <chrono>

namespace HephAudio
{
	void ConsoleLogger::Log(const char* message, const char* logLevel)
	{
#if defined(_WIN32) || defined(__linux__)

		printf("\u001b[%smHephAudio[%s]: \033[0m%s", logLevel, ConsoleLogger::CurrentTimeToString().c_str(), message);

#endif
	}
	void ConsoleLogger::LogLine(const char* message, const char* logLevel)
	{
#if defined(_WIN32) || defined(__linux__)

		printf("\u001b[%smHephAudio[%s]: \033[0m%s\n", logLevel, ConsoleLogger::CurrentTimeToString().c_str(), message);

#elif defined(__ANDROID__)

		ConsoleLogger::Log(message, logLevel);

#endif
	}

	std::string ConsoleLogger::CurrentTimeToString()
	{
		constexpr uint8_t timeStringSize = 9;

		time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		time(&currentTime);

		tm* localTime = localtime(&currentTime);
		
		char timeString[timeStringSize];
		strftime(timeString, timeStringSize, "%T", localTime);

		return timeString;
	}
}