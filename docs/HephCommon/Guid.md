## Guid class
```c++
#include <Guid.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)



### Description
Struct for storing Globally Unique Identifiers.
<br><br>



### Fields

```c++
struct Guid
{
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
}
```

- ``data1``
<br><br>
First part of the GUID.
<br><br>

- ``data2``
<br><br>
Second part of the GUID.
<br><br>

- ``data3``
<br><br>
Third part of the GUID.
<br><br>

- ``data4``
<br><br>
Fourth part of the GUID.
<br><br>



### Methods

```c++
constexpr Guid();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
constexpr bool operator==(const Guid& rhs) const;
```
Compares two instances.
- **rhs:** right hand side.
- **returns:** ``true`` if two instances are equal, otherwise ``false``.
<br><br><br><br>

```c++
constexpr bool operator!=(const Guid& rhs) const;
```
Compares two instances.
- **rhs:** right hand side.
- **returns:** ``true`` if two instances are not equal, otherwise ``false``.
<br><br><br><br>

```c++
static Guid GenerateNew();
```
Generates a random GUID.
- **returns:** GUID.



