## ConsoleLogger class
```c++
#include <ConsoleLogger.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Methods](#methods)<br>


### Description
Class for printing messages to console.

> [!NOTE]
> This class only contains static methods and cannot be instantiated.

<br><br>



### Macros

```c++
#define HEPH_CL_INFO
#define HEPH_CL_WARNING
#define HEPH_CL_ERROR
#define HEPH_CL_SUCCESS
#define HEPH_CL_DEBUG
```
Log levels.

<br><br>

### Methods

```c++
static void Log(const std::string& message,
                const char* logLevel);
```
Prints the provided message to the console.
- **message:** Message that will be printed.
- **logLevel:** Log level.
<br><br><br><br>

```c++
static void Log(const std::string& message,
                const char* logLevel,
                std::string libName);
```
Prints the provided message to the console.
- **message:** Message that will be printed.
- **logLevel:** Log level.
- **libName:** Name of the library or application that prints the message.
<br><br><br><br>

```c++
static void LogInfo(const std::string& message);
```
Prints the provided message to the console with log level set to info.
- **message:** Message that will be printed.
<br><br><br><br>

```c++
static void LogInfo(const std::string& message,
                    std::string libName);
```
Prints the provided message to the console with log level set to info.
- **message:** Message that will be printed.
- **libName:** Name of the library or application that prints the message.
<br><br><br><br>

```c++
static void LogWarning(const std::string& message);
```
Prints the provided message to the console with log level set to warning.
- **message:** Message that will be printed.
<br><br><br><br>

```c++
static void LogWarning(const std::string& message,
                       std::string libName);
```
Prints the provided message to the console with log level set to warning.
- **message:** Message that will be printed.
- **libName:** Name of the library or application that prints the message.
<br><br><br><br>

```c++
static void LogError(const std::string& message);
```
Prints the provided message to the console with log level set to error.
- **message:** Message that will be printed.
<br><br><br><br>

```c++
static void LogError(const std::string& message,
                     std::string libName);
```
Prints the provided message to the console with log level set to error.
- **message:** Message that will be printed.
- **libName:** Name of the library or application that prints the message.
<br><br><br><br>

```c++
static void LogSuccess(const std::string& message);
```
Prints the provided message to the console with log level set to success.
- **message:** Message that will be printed.
<br><br><br><br>

```c++
static void LogSuccess(const std::string& message,
                       std::string libName);
```
Prints the provided message to the console with log level set to success.
- **message:** Message that will be printed.
- **libName:** Name of the library or application that prints the message.
<br><br><br><br>

```c++
static void LogDebug(const std::string& message);
```
Prints the provided message to the console with log level set to debug.
- **message:** Message that will be printed.
<br><br><br><br>

```c++
static void LogDebug(const std::string& message,
                     std::string libName);
```
Prints the provided message to the console with log level set to debug.
- **message:** Message that will be printed.
- **libName:** Name of the library or application that prints the message.
<br><br><br><br>

```c++
static void EnableColoredOutput();
```
Enables the colored output. Colored output is enabled by default.
<br><br><br><br>

```c++
static void DisableColoredOutput();
```
Disables the colored output.
<br><br><br><br>
