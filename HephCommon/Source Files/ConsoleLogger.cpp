#include "ConsoleLogger.h"
#include <stdio.h>
#include <chrono>
#if defined(__ANDROID__)
#include <android/log.h>
#endif

namespace HephCommon
{
	void ConsoleLogger::Log(StringBuffer message, const char* logLevel)
	{
		ConsoleLogger::Log(message, logLevel, "HephLibs");
	}
	void ConsoleLogger::Log(StringBuffer message, const char* logLevel, StringBuffer libName)
	{
#if defined(_WIN32) || (defined(__linux__) && ! defined(__ANDROID__))

		if (message.GetStringType() == StringType::ASCII)
		{
			printf("\u001b[%sm%s[%s]: \033[0m%s\n", logLevel, libName.fc_str(), ConsoleLogger::CurrentTimeToString(StringType::ASCII).c_str(), message.c_str());
		}
		else
		{
			wprintf(L"\u001b[%sm%s[%s]: \033[0m%s\n", StringBuffer(logLevel, StringType::Wide).wc_str(), libName.fwc_str(), ConsoleLogger::CurrentTimeToString(StringType::Wide).wc_str(), message.wc_str());
		}

#elif defined(__ANDROID__)

		if (message.GetStringType() == StringType::ASCII)
		{
			if (logLevel == ConsoleLogger::info)
			{
				__android_log_print(ANDROID_LOG_INFO, libName.fc_str(), "%s", message.c_str());
			}
			else if (logLevel == ConsoleLogger::warning)
			{
				__android_log_print(ANDROID_LOG_WARN, libName.fc_str(), "%s", message.c_str());
			}
			else if (logLevel == ConsoleLogger::error)
			{
				__android_log_print(ANDROID_LOG_ERROR, libName.fc_str(), "%s", message.c_str());
			}
			else if (logLevel == ConsoleLogger::success)
			{
				__android_log_print(ANDROID_LOG_VERBOSE, libName.fc_str(), "%s", message.c_str());
			}
			else if (logLevel == ConsoleLogger::debug)
			{
				__android_log_print(ANDROID_LOG_DEBUG, libName.fc_str(), "%s", message.c_str());
			}
			else
			{
				__android_log_print(ANDROID_LOG_DEFAULT, libName.fc_str(), "%s", message.c_str());
			}
		}
		else
		{
			if (logLevel == ConsoleLogger::info)
			{
				__android_log_print(ANDROID_LOG_INFO, libName.fc_str(), "%ls", message.wc_str());
			}
			else if (logLevel == ConsoleLogger::warning)
			{
				__android_log_print(ANDROID_LOG_WARN, libName.fc_str(), "%ls", message.wc_str());
			}
			else if (logLevel == ConsoleLogger::error)
			{
				__android_log_print(ANDROID_LOG_ERROR, libName.fc_str(), "%ls", message.wc_str());
			}
			else if (logLevel == ConsoleLogger::success)
			{
				__android_log_print(ANDROID_LOG_VERBOSE, libName.fc_str(), "%ls", message.wc_str());
			}
			else if (logLevel == ConsoleLogger::debug)
			{
				__android_log_print(ANDROID_LOG_DEBUG, libName.fc_str(), "%ls", message.wc_str());
			}
			else
			{
				__android_log_print(ANDROID_LOG_DEFAULT, libName.fc_str(), "%ls", message.wc_str());
			}
		}

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