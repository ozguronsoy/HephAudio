## HephMath class
```c++
#include <HephMath.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Fields](#fields)<br>
[Methods](#methods)


### Description
Defines the constexpr versions of some of the math functions.
<br><br>

> [!NOTE]
> This class only contains static methods and cannot be instantiated.



### Macros

```c++
#define HEPH_MATH_PI
#define HEPH_MATH_E
```
<br><br>


### Fields

```c++
class HephMath final
{
    static constexpr long double pi_l;
    static constexpr double pi;
    static constexpr float pi_f;
    static constexpr heph_float pi_hf;
    static constexpr long double e_l;
    static constexpr double e;
    static constexpr float e_f;
    static constexpr heph_float e_hf;
};
```
<br><br>


### Methods

```c++
static constexpr float Sgn(float x);
```
<br><br><br><br>

```c++
static constexpr float Max(float a, float b);
```
<br><br><br><br>

```c++
static constexpr float Min(float a, float b);
```
<br><br><br><br>

```c++
static constexpr float Abs(float x);
```
<br><br><br><br>

```c++
static constexpr float Round(float x);
```
<br><br><br><br>

```c++
static constexpr float Floor(float x);
```
<br><br><br><br>

```c++
static constexpr float Ceil(float x);
```
<br><br><br><br>

```c++
static constexpr float Mod(float a, float b);
```
<br><br><br><br>

```c++
static constexpr float RadToDeg(float x);
```
<br><br><br><br>

```c++
static constexpr float DegToRad(float x);
```
<br><br><br><br>
