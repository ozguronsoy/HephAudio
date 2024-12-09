#pragma once
#include "HephShared.h"
#include <string>

/** @file */

#define HEPH_CL_INFO 	"36"
#define HEPH_CL_WARNING "33"
#define HEPH_CL_ERROR 	"31"
#define HEPH_CL_SUCCESS "32"
#define HEPH_CL_DEBUG 	"35"

namespace Heph
{
	/**
	 * @brief class for printing formatted messages to the console.
	 * @note this is a static class and cannot be instantiated.
	 */
	class HEPH_API ConsoleLogger final
	{
	private:
#if defined(HEPH_CL_DISABLE_COLORED_OUTPUT)
		static inline bool coloredOutput = false;
#else
		static inline bool coloredOutput = true;
#endif

	public:
		ConsoleLogger() = delete;
		ConsoleLogger(const ConsoleLogger&) = delete;
		ConsoleLogger& operator=(const ConsoleLogger&) = delete;

	public:
		/**
		 * prints the provided message to the console.
		 * 
		 * @param message message that will be printed.
		 * @param logLevel one of the <b>HEPH_CL_*</b> macros. 
		 * 
		 */
		static void Log(const std::string& message, const char* logLevel);

		/**
		 * prints the provided message to the console.
		 * 
		 * @param message message that will be printed.
		 * @param logLevel one of the <b>HEPH_CL_*</b> macros.
		 * @param libName name of the library thats printing. The default value is "HephLibs".
		 * 
		 */
		static void Log(const std::string& message, const char* logLevel, const std::string& libName);

		/**
		 * prints the provided message to the console as INFO.
		 *
		 * @param message message that will be printed.
		 *
		 */
		static void LogInfo(const std::string& message);

		/**
		 * prints the provided message to the console as INFO.
		 *
		 * @param message message that will be printed.
		 * @param libName name of the library thats printing. The default value is "HephLibs".
		 *
		 */
		static void LogInfo(const std::string& message, const std::string& libName);

		/**
		 * prints the provided message to the console as WARNING.
		 *
		 * @param message message that will be printed.
		 *
		 */
		static void LogWarning(const std::string& message);

		/**
		 * prints the provided message to the console as WARNING.
		 *
		 * @param message message that will be printed.
		 * @param libName name of the library thats printing. The default value is "HephLibs".
		 *
		 */
		static void LogWarning(const std::string& message, const std::string& libName);

		/**
		 * prints the provided message to the console as ERROR.
		 *
		 * @param message message that will be printed.
		 *
		 */
		static void LogError(const std::string& message);

		/**
		 * prints the provided message to the console as ERROR.
		 *
		 * @param message message that will be printed.
		 * @param libName name of the library thats printing. The default value is "HephLibs".
		 *
		 */
		static void LogError(const std::string& message, const std::string& libName);

		/**
		 * prints the provided message to the console as SUCCESS.
		 *
		 * @param message message that will be printed.
		 *
		 */
		static void LogSuccess(const std::string& message);

		/**
		 * prints the provided message to the console as SUCCESS.
		 *
		 * @param message message that will be printed.
		 * @param libName name of the library thats printing. The default value is "HephLibs".
		 *
		 */
		static void LogSuccess(const std::string& message, const std::string& libName);

		/**
		 * prints the provided message to the console as DEBUG.
		 *
		 * @param message message that will be printed.
		 *
		 */
		static void LogDebug(const std::string& message);

		/**
		 * prints the provided message to the console as DEBUG.
		 *
		 * @param message message that will be printed.
		 * @param libName name of the library thats printing. The default value is "HephLibs".
		 *
		 */
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