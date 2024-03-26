## StopWatch class
```c++
#include <StopWatch.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Methods](#methods)<br>


### Description
Class for measuring time.
<br><br>



### Macros

```c++
#define HEPH_SW_NANO
#define HEPH_SW_MICRO
#define HEPH_SW_MILLI
```
SI prefixes.

<br><br>



### Methods

```c++
StopWatch();
```
Creates an instance and initializes with the default values.
<br><br><br><br>

```c++
void Start();
```
Starts the stopwatch.
<br><br><br><br>

```c++
void Reset();
```
Restarts the stopwatch.
<br><br><br><br>

```c++
double DeltaTime() const;
```
Calculates the elapsed time since the stopwatch is started.
- **returns:** The elapsed time.
<br><br><br><br>

```c++
double DeltaTime(double prefix) const;
```
Calculates the elapsed time since the stopwatch is started.
- **prefix:** SI prefix.
- **returns:** The elapsed time.
<br><br><br><br>

```c++
void Stop();
```
Stops the stopwatch.
<br><br><br><br>

```c++
static void StaticStart();
```
Starts the stopwatch. This method is thread-safe.
<br><br><br><br>

```c++
static void StaticReset();
```
Restarts the stopwatch. This method is thread-safe.
<br><br><br><br>

```c++
static double StaticDeltaTime() const;
```
Calculates the elapsed time since the stopwatch is started. This method is thread-safe.
- **returns:** The elapsed time.
<br><br><br><br>

```c++
static double StaticDeltaTime(double prefix) const;
```
Calculates the elapsed time since the stopwatch is started. This method is thread-safe.
- **prefix:** SI prefix.
- **returns:** The elapsed time.
<br><br><br><br>

```c++
static void Stop();
```
Stops the stopwatch. This method is thread-safe.
<br><br><br><br>
