#pragma once
#include "HephCommonShared.h"
#include <string>

/** @file */

#define HEPH_CL_INFO 	"36"
#define HEPH_CL_WARNING "33"
#define HEPH_CL_ERROR 	"31"
#define HEPH_CL_SUCCESS "32"
#define HEPH_CL_DEBUG 	"35"

namespace HephCommon
{
	/**
	 * @brief class for printing formatted messages to the console.
	 * @note this class cannot be instantiated.
	 */
	class ConsoleLogger final
	{
	private:
		static inline bool coloredOutput = true;

	public:
		ConsoleLogger() = delete;
		ConsoleLogger(const ConsoleLogger&) = delete;
		ConsoleLogger& operator=(const ConsoleLogger&) = delete;

	public:
		/**
		 * prints the provided message to the console.
		 * 
		 * @param logLevel one of the <b>HEPH_CL_*</b> macros. 
		 */
		static void Log(const std::string& message, const char* logLevel);

		/**
		 * prints the provided message to the console.
		 * 
		 * @param logLevel one of the <b>HEPH_CL_*</b> macros. 
		 * @param libName name of the library thats printing. The default value is "HephLibs".
		 */
		static void Log(const std::string& message, const char* logLevel, const std::string& libName);
		static void LogInfo(const std::string& message);
		static void LogInfo(const std::string& message, const std::string& libName);
		static void LogWarning(const std::string& message);
		static void LogWarning(const std::string& message, const std::string& libName);
		static void LogError(const std::string& message);
		static void LogError(const std::string& message, const std::string& libName);
		static void LogSuccess(const std::string& message);
		static void LogSuccess(const std::string& message, const std::string& libName);
		static void LogDebug(const std::string& message);
		static void LogDebug(const std::string& message, const std::string& libName);

		/**
		 * enables the use of ASCII color codes.
		 * 
		 */
		static void EnableColoredOutput();

		/**
		 * disables the use of ASCII color codes.
		 * 
		 */
		static void DisableColoredOutput();

	private:
		static std::string CurrentTimeToString();
		static std::string GetLogLevelName(const char* logLevel);
	};
}