#include "ConsoleLogger.h"
#include <stdio.h>
#include <chrono>
#if defined(__ANDROID__)
#include <android/log.h>
#endif

namespace HephCommon
{
	bool ConsoleLogger::coloredOutput = true;

	void ConsoleLogger::Log(const std::string& message, const char* logLevel)
	{
		ConsoleLogger::Log(message, logLevel, "HephLibs");
	}
	void ConsoleLogger::Log(const std::string& message, const char* logLevel, const std::string& libName)
	{
#if defined(__ANDROID__)

		if (strcmp(logLevel, HEPH_CL_INFO) == 0)
		{
			__android_log_print(ANDROID_LOG_INFO, libName.c_str(), "%s", message.c_str());
		}
		else if (strcmp(logLevel, HEPH_CL_WARNING) == 0)
		{
			__android_log_print(ANDROID_LOG_WARN, libName.c_str(), "%s", message.c_str());
		}
		else if (strcmp(logLevel, HEPH_CL_ERROR) == 0)
		{
			__android_log_print(ANDROID_LOG_ERROR, libName.c_str(), "%s", message.c_str());
		}
		else if (strcmp(logLevel, HEPH_CL_SUCCESS) == 0)
		{
			__android_log_print(ANDROID_LOG_VERBOSE, libName.c_str(), "%s", message.c_str());
		}
		else if (strcmp(logLevel, HEPH_CL_DEBUG) == 0)
		{
			__android_log_print(ANDROID_LOG_DEBUG, libName.c_str(), "%s", message.c_str());
		}
		else
		{
			__android_log_print(ANDROID_LOG_DEFAULT, libName.c_str(), "%s", message.c_str());
		}

#else

		if (ConsoleLogger::coloredOutput)
		{
			printf("\x1b[%sm%s[%s]: \x1b[0m%s\n", logLevel, libName.c_str(), ConsoleLogger::CurrentTimeToString().c_str(), message.c_str());
		}
		else
		{
			printf("%s[%s][%s]: %s\n", libName.c_str(), ConsoleLogger::CurrentTimeToString().c_str(), ConsoleLogger::GetLogLevelName(logLevel).c_str(), message.c_str());
		}

#endif
	}
	void ConsoleLogger::LogInfo(const std::string& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_INFO);
	}
	void ConsoleLogger::LogInfo(const std::string& message, const std::string& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_INFO, libName);
	}
	void ConsoleLogger::LogWarning(const std::string& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_WARNING);
	}
	void ConsoleLogger::LogWarning(const std::string& message, const std::string& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_WARNING, libName);
	}
	void ConsoleLogger::LogError(const std::string& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_ERROR);
	}
	void ConsoleLogger::LogError(const std::string& message, const std::string& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_ERROR, libName);
	}
	void ConsoleLogger::LogSuccess(const std::string& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_SUCCESS);
	}
	void ConsoleLogger::LogSuccess(const std::string& message, const std::string& libName)
	{
		ConsoleLogger::Log(message, HEPH_CL_SUCCESS, libName);
	}
	void ConsoleLogger::LogDebug(const std::string& message)
	{
		ConsoleLogger::Log(message, HEPH_CL_DEBUG);
	}
	void ConsoleLogger::LogDebug(const std::string& message, const std::string& libName)
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
	std::string ConsoleLogger::CurrentTimeToString()
	{
		constexpr uint8_t timeStringSize = 10;

		time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		time(&currentTime);

		tm* localTime = localtime(&currentTime);

		char timeString[timeStringSize];
		strftime(timeString, timeStringSize, "%T", localTime);

		return std::string(timeString);
	}
	std::string ConsoleLogger::GetLogLevelName(const char* logLevel)
	{
		if (strcmp(logLevel, HEPH_CL_INFO) == 0)
		{
			return std::string("INFO");
		}
		else if (strcmp(logLevel, HEPH_CL_WARNING) == 0)
		{
			return std::string("WARNING");
		}
		else if (strcmp(logLevel, HEPH_CL_ERROR) == 0)
		{
			return std::string("ERROR");
		}
		else if (strcmp(logLevel, HEPH_CL_SUCCESS) == 0)
		{
			return std::string("SUCCESS");
		}
		else if (strcmp(logLevel, HEPH_CL_DEBUG) == 0)
		{
			return std::string("DEBUG");
		}
		return std::string("UNKNOWN");
	}
}