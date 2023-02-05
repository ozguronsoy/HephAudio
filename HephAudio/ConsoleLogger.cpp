#include "ConsoleLogger.h"
#include <stdio.h>
#include <chrono>
#if defined(__ANDROID__)
#include <android/log.h>
#endif

namespace HephAudio
{
	void ConsoleLogger::Log(StringBuffer message, const char* logLevel)
	{
#if defined(_WIN32) || (defined(__linux__) && ! defined(__ANDROID__))

		if (message.GetStringType() == StringType::Normal)
		{
			printf("\u001b[%smHephAudio[%s]: \033[0m%s", logLevel, ConsoleLogger::CurrentTimeToString(StringType::Normal).c_str(), message.c_str());
		}
		else
		{
			wprintf(L"\u001b[%smHephAudio[%s]: \033[0m%s", StringBuffer(logLevel, StringType::Wide).wc_str(), ConsoleLogger::CurrentTimeToString(StringType::Wide).wc_str(), message.wc_str());
		}

#elif defined(__ANDROID__)

		if (message.GetStringType() == StringType::Normal)
		{
			if (logLevel == ConsoleLogger::info)
			{
				__android_log_print(ANDROID_LOG_INFO, "HephAudio", "%s", message.c_str());
			}
			else if (logLevel == ConsoleLogger::warning)
			{
				__android_log_print(ANDROID_LOG_WARN, "HephAudio", "%s", message.c_str());
			}
			else if (logLevel == ConsoleLogger::error)
			{
				__android_log_print(ANDROID_LOG_ERROR, "HephAudio", "%s", message.c_str());
			}
			else if (logLevel == ConsoleLogger::success)
			{
				__android_log_print(ANDROID_LOG_VERBOSE, "HephAudio", "%s", message.c_str());
			}
			else if (logLevel == ConsoleLogger::debug)
			{
				__android_log_print(ANDROID_LOG_DEBUG, "HephAudio", "%s", message.c_str());
			}
			else
			{
				__android_log_print(ANDROID_LOG_DEFAULT, "HephAudio", "%s", message.c_str());
			}
		}
		else
		{
			if (logLevel == ConsoleLogger::info)
			{
				__android_log_print(ANDROID_LOG_INFO, "HephAudio", "%ls", message.wc_str());
			}
			else if (logLevel == ConsoleLogger::warning)
			{
				__android_log_print(ANDROID_LOG_WARN, "HephAudio", "%ls", message.wc_str());
			}
			else if (logLevel == ConsoleLogger::error)
			{
				__android_log_print(ANDROID_LOG_ERROR, "HephAudio", "%ls", message.wc_str());
			}
			else if (logLevel == ConsoleLogger::success)
			{
				__android_log_print(ANDROID_LOG_VERBOSE, "HephAudio", "%ls", message.wc_str());
			}
			else if (logLevel == ConsoleLogger::debug)
			{
				__android_log_print(ANDROID_LOG_DEBUG, "HephAudio", "%ls", message.wc_str());
			}
			else
			{
				__android_log_print(ANDROID_LOG_DEFAULT, "HephAudio", "%ls", message.wc_str());
			}
		}

#endif
	}
	void ConsoleLogger::LogLine(StringBuffer message, const char* logLevel)
	{
#if defined(_WIN32) || (defined(__linux__) && ! defined(__ANDROID__))

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