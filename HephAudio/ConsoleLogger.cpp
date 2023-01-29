#include "ConsoleLogger.h"
#include <stdio.h>
#include <chrono>

namespace HephAudio
{
	void ConsoleLogger::Log(StringBuffer message, const char* logLevel)
	{
#if defined(_WIN32) || defined(__linux__)
		
		if (message.GetStringType() == StringType::Normal)
		{
			printf("\u001b[%smHephAudio[%s]: \033[0m%s", logLevel, ConsoleLogger::CurrentTimeToString(StringType::Normal).c_str(), message.c_str());
		}
		else
		{
			wprintf(L"\u001b[%smHephAudio[%s]: \033[0m%s", StringBuffer(logLevel, StringType::Wide).wc_str(), ConsoleLogger::CurrentTimeToString(StringType::Wide).wc_str(), message.wc_str());
		}

#endif
	}
	void ConsoleLogger::LogLine(StringBuffer message, const char* logLevel)
	{
#if defined(_WIN32) || defined(__linux__)

		if (message.GetStringType() == StringType::Normal)
		{
			printf("\u001b[%smHephAudio[%s]: \033[0m%s\n", logLevel, ConsoleLogger::CurrentTimeToString(StringType::Normal).c_str(), message.c_str());
		}
		else
		{
			wprintf(L"\u001b[%smHephAudio[%s]: \033[0m%s\n", StringBuffer(logLevel, StringType::Wide).wc_str(), ConsoleLogger::CurrentTimeToString(StringType::Wide).wc_str(), message.wc_str());
		}

#elif defined(__ANDROID__)

		ConsoleLogger::Log(message, logLevel);

#endif
	}

	StringBuffer ConsoleLogger::CurrentTimeToString(StringType stringType)
	{
		constexpr uint8_t timeStringSize = 10;

		time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		time(&currentTime);

		tm* localTime = localtime(&currentTime);

		char timeString[timeStringSize];
		strftime(timeString, timeStringSize, "%T", localTime);

		return StringBuffer(timeString, stringType);
	}
}