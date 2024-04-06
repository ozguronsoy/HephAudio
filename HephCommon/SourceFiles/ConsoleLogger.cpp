#include "ConsoleLogger.h"
#include <stdio.h>
#include <chrono>
#if defined(__ANDROID__)
#include <android/log.h>
#endif

namespace HephCommon
{
	bool ConsoleLogger::coloredOutput = true;

	void ConsoleLogger::Log(StringBuffer message, const char* logLevel)
	{
		ConsoleLogger::Log(message, logLevel, "HephLibs");
	}
	void ConsoleLogger::Log(StringBuffer message, const char* logLevel, StringBuffer libName)
	{
#if defined(__ANDROID__)

		if (message.GetStringType() == StringType::ASCII)
		{
			if (strcmp(logLevel, HEPH_CL_INFO))
			{
				__android_log_print(ANDROID_LOG_INFO, libName.fc_str(), "%s", message.c_str());
			}
			else if (strcmp(logLevel, HEPH_CL_WARNING))
			{
				__android_log_print(ANDROID_LOG_WARN, libName.fc_str(), "%s", message.c_str());
			}
			else if (strcmp(logLevel, HEPH_CL_ERROR))
			{
				__android_log_print(ANDROID_LOG_ERROR, libName.fc_str(), "%s", message.c_str());
			}
			else if (strcmp(logLevel, HEPH_CL_SUCCESS))
			{
				__android_log_print(ANDROID_LOG_VERBOSE, libName.fc_str(), "%s", message.c_str());
			}
			else if (strcmp(logLevel, HEPH_CL_DEBUG))
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
			if (strcmp(logLevel, HEPH_CL_INFO))
			{
				__android_log_print(ANDROID_LOG_INFO, libName.fc_str(), "%ls", message.wc_str());
			}
			else if (strcmp(logLevel, HEPH_CL_WARNING))
			{
				__android_log_print(ANDROID_LOG_WARN, libName.fc_str(), "%ls", message.wc_str());
			}
			else if (strcmp(logLevel, HEPH_CL_ERROR))
			{
				__android_log_print(ANDROID_LOG_ERROR, libName.fc_str(), "%ls", message.wc_str());
			}
			else if (strcmp(logLevel, HEPH_CL_SUCCESS))
			{
				__android_log_print(ANDROID_LOG_VERBOSE, libName.fc_str(), "%ls", message.wc_str());
			}
			else if (strcmp(logLevel, HEPH_CL_DEBUG))
			{
				__android_log_print(ANDROID_LOG_DEBUG, libName.fc_str(), "%ls", message.wc_str());
			}
			else
			{
				__android_log_print(ANDROID_LOG_DEFAULT, libName.fc_str(), "%ls", message.wc_str());
			}
		}

#else

		if (ConsoleLogger::coloredOutput)
		{
			if (message.GetStringType() == StringType::ASCII)
			{
				printf("\x1b[%sm%s[%s]: \x1b[0m%s\n", logLevel, libName.fc_str(), ConsoleLogger::CurrentTimeToString(StringType::ASCII).c_str(), message.c_str());
			}
			else
			{
				printf("\x1b[%lsm%ls[%ls]: \x1b[0m%ls\n", StringBuffer(logLevel, StringType::Wide).wc_str(), libName.fwc_str(), ConsoleLogger::CurrentTimeToString(StringType::Wide).wc_str(), message.wc_str());
			}
		}
		else
		{
			if (message.GetStringType() == StringType::ASCII)
			{
				printf("%s[%s][%s]: %s\n", libName.fc_str(), ConsoleLogger::CurrentTimeToString(StringType::ASCII).c_str(), ConsoleLogger::GetLogLevelName(logLevel, StringType::ASCII).c_str(), message.c_str());
			}
			else
			{
				printf("%ls[%ls][%ls]: %ls\n", libName.fwc_str(), ConsoleLogger::CurrentTimeToString(StringType::Wide).wc_str(), ConsoleLogger::GetLogLevelName(logLevel, StringType::Wide).wc_str(), message.wc_str());
			}
		}

#endif
	}
	void ConsoleLogger::LogInfo(const StringBuffer& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_INFO);
	}
	void ConsoleLogger::LogInfo(const StringBuffer& message, const StringBuffer& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_INFO, libName);
	}
	void ConsoleLogger::LogWarning(const StringBuffer& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_WARNING);
	}
	void ConsoleLogger::LogWarning(const StringBuffer& message, const StringBuffer& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_WARNING, libName);
	}
	void ConsoleLogger::LogError(const StringBuffer& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_ERROR);
	}
	void ConsoleLogger::LogError(const StringBuffer& message, const StringBuffer& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_ERROR, libName);
	}
	void ConsoleLogger::LogSuccess(const StringBuffer& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_SUCCESS);
	}
	void ConsoleLogger::LogSuccess(const StringBuffer& message, const StringBuffer& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_SUCCESS, libName);
	}
	void ConsoleLogger::LogDebug(const StringBuffer& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_DEBUG);
	}
	void ConsoleLogger::LogDebug(const StringBuffer& message, const StringBuffer& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_DEBUG, libName);
	}
	void ConsoleLogger::EnableColoredOutput()
	{
		ConsoleLogger::coloredOutput = true;
	}
	void ConsoleLogger::DisableColoredOutput()
	{
		ConsoleLogger::coloredOutput = false;
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
	StringBuffer ConsoleLogger::GetLogLevelName(const char* logLevel, StringType stringType)
	{
		if (strcmp(logLevel, HEPH_CL_INFO) == 0)
		{
			return StringBuffer("INFO", stringType);
		}
		else if (strcmp(logLevel, HEPH_CL_WARNING) == 0)
		{
			return StringBuffer("WARNING", stringType);
		}
		else if (strcmp(logLevel, HEPH_CL_ERROR) == 0)
		{
			return StringBuffer("ERROR", stringType);
		}
		else if (strcmp(logLevel, HEPH_CL_SUCCESS) == 0)
		{
			return StringBuffer("SUCCESS", stringType);
		}
		else if (strcmp(logLevel, HEPH_CL_DEBUG) == 0)
		{
			return StringBuffer("DEBUG", stringType);
		}
		return StringBuffer("UNKNOWN", stringType);
	}
}