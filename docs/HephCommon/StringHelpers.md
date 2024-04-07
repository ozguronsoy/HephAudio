## StringHelpers class
```c++
#include <StringHelpers.h>
using namespace HephCommon;
```


> [Description](#description)<br>
[Methods](#methods)





### Description
Defines useful methods for string operations.
<br><br>


> [!NOTE]
> This class only contains static methods and cannot be instantiated.

<br><br>


### Methods


```c++
static std::wstring StrToWide(const std::string& str);
```
Converts ``std::string`` to ``std::wstring``.
<br><br><br><br>

```c++
static std::string WideToStr(const std::wstring& wstr);
```
Converts ``std::wstring`` to ``std::string``.
<br><br><br><br>

```c++
static std::vector<std::string> Split(const std::string& str,
                                      const std::string& separator);
```
Splits the string into substrings delimited by the ``separator``.
<br><br><br><br>

```c++
static std::vector<std::wstring> Split(const std::wstring& wstr,
                                       const std::wstring& separator);
```
Splits the string into substrings delimited by the ``separator``.
<br><br><br><br>

```c++
static std::string ToString(double value);
```
Converts the numerical value to its string representation.<br>
This method has overloads for ``intXX_t`` and ``uintXX_t`` types.
<br><br><br><br>

```c++
static std::string ToString(double value,
                            size_t precision);
```
Converts the numerical value to its string representation with the provided precision after the decimal point.
<br><br><br><br>

```c++
static std::string ToString(const Guid& guid);
```
Converts the [GUID](/docs/HephCommon/Guid.md) to its string representation.
<br><br><br><br>

```c++
static std::string ToHexString(uint32_t value);
```
Converts the numerical value to its hex string representation.<br>
This method has overloads for ``intXX_t`` and ``uintXX_t`` types.
<br><br><br><br>

```c++
static int16_t StringToS16(const std::string& string);
```
Converts string representation to numerical value.
<br><br><br><br>

```c++
static uint16_t StringToU16(const std::string& string);
```
Converts string representation to numerical value.
<br><br><br><br>

```c++
static int32_t StringToS32(const std::string& string);
```
Converts string representation to numerical value.
<br><br><br><br>

```c++
static uint32_t StringToU32(const std::string& string);
```
Converts string representation to numerical value.
<br><br><br><br>

```c++
static int64_t StringToS64(const std::string& string);
```
Converts string representation to numerical value.
<br><br><br><br>

```c++
static uint64_t StringToU64(const std::string& string);
```
Converts string representation to numerical value.
<br><br><br><br>

```c++
static double StringToDouble(const std::string& string);
```
Converts string representation to numerical value.
<br><br><br><br>

```c++
static Guid StringToGuid(const std::string& string);
```
Converts string representation to [GUID](/docs/HephCommon/Guid.md).
<br><br><br><br>

```c++
static int16_t HexStringToS16(const std::string& hexString);
```
Converts hex string representation to numerical value.
<br><br><br><br>

```c++
static uint16_t HexStringToU16(const std::string& hexString);
```
Converts hex string representation to numerical value.
<br><br><br><br>

```c++
static int32_t HexStringToS32(const std::string& hexString);
```
Converts hex string representation to numerical value.
<br><br><br><br>

```c++
static uint32_t HexStringToU32(const std::string& hexString);
```
Converts hex string representation to numerical value.
<br><br><br><br>

```c++
static int64_t HexStringToS64(const std::string& hexString);
```
Converts hex string representation to numerical value.
<br><br><br><br>

```c++
static uint64_t HexStringToU64(const std::string& hexString);
```
Converts hex string representation to numerical value.
