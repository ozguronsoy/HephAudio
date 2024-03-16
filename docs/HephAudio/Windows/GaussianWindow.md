## GaussianWindow Class
```c++
#include <Windows/GaussianWindow.h>
using namespace HephAudio;
```
**Inheritance:** *[Window](/docs/HephAudio/Windows/Window.md)* -> *GaussianWindow*

### Methods
```c++
GaussianWindow();
```
Creates an instance and initializes it with default values.
<br><br><br><br>
```c++
GaussianWindow(size_t size);
```
Creates an instance and initializes it with provided values.
- **size:** Size of the window in frames.
<br><br><br><br>
```c++
GaussianWindow(size_t size, heph_float sigma);
```
Creates an instance and initializes it with provided values.
- **size:** Size of the window in frames.
- **sigma:** Width factor.
<br><br><br><br>
```c++
heph_float GetSigma() const;
```
Gets the width factor.
- **Returns:** The width factor.
<br><br><br><br>

```c++
void SetSigma(heph_float sigma);
```
Sets the width factor.
- **sigma:** The new width factor.
