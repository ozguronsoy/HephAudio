## HephCommonShared
```c++
#include <HephCommonShared.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Enums](#enums)<br>
[Data Types](#data-types)<br>
[Methods](#methods)


### Description
Defines macros, enums, data types, and methods that will be used in all HephCommon header files.
<br><br>


### Macros

```c++
#define CPP_VERSION
```
Targeted C++ version.
<br><br>

```c++
#define CPP_VERSION_PRE_98
#define CPP_VERSION_98
#define CPP_VERSION_11
#define CPP_VERSION_14
#define CPP_VERSION_17
#define CPP_VERSION_20
#define CPP_VERSION_23
```
C++ versions.
<br><br>

```c++
#define HEPH_CONSTEVAL
```
Expands to ``consteval`` for C++ 20 and above, otherwise ``constexpr``.
<br><br>

```c++
#define HEPH_FLOAT
```
<br><br>

```c++
#define HEPH_FLOAT
#define HEPH_ENDIAN
```
<br><br>

```c++
#define HEPH_FLOAT heph_float
#define HEPH_ENDIAN HephCommon::Endian
#define HEPH_CONVOLUTION_MODE HephCommon::ConvolutionMode
```
<br><br>

```c++
#define HEPH_SYSTEM_ENDIAN
```
Gets the endianness of the current system.
<br><br>

```c++
#define HEPH_CHANGE_ENDIAN(pData, dataSize)
```
Changes the endianness of the provided data.
- **pData:** Pointer to the data.
- **dataSize:** Size of the data in bytes.
<br><br>


### Enums

```c++
enum Endian : uint8_t
{
	Little = 0x00,
	Big = 0x01,
	Unknown = 0xFF
};
```
<br><br>

```c++
enum ConvolutionMode
{
	Full = 0,
	Central = 1,
	ValidPadding = 2
};
```
<br><br>


### Data Types

```c++
typedef float heph_float;
```
Float type used internally, ``float`` by default. 
Can be changed to ``double`` by defining ``HEPH_HIGH_PRECISION_FLOAT``
<br><br>


### Methods

```c++
void ChangeEndian(uint8_t* pData, uint8_t dataSize);
```
Same as the ``HEPH_CHANGE_ENDIAN`` macro.
