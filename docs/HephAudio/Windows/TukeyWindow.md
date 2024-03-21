## TukeyWindow Class
```c++
#include <Windows/TukeyWindow.h>
using namespace HephAudio;
```
**Inheritance:** *[Window](/docs/HephAudio/Windows/Window.md)* -> *TukeyWindow*

### Methods
```c++
TukeyWindow();
```
Creates an instance and initializes it with default values.
<br><br><br><br>
```c++
TukeyWindow(size_t size);
```
Creates an instance and initializes it with provided values.
- **size:** Size of the window in frames.
<br><br><br><br>
```c++
TukeyWindow(size_t size,
            heph_float alpha);
```
Creates an instance and initializes it with provided values.
- **size:** Size of the window in frames.
- **alpha:** The alpha factor.
<br><br><br><br>
```c++
heph_float GetAlpha() const;
```
Gets the alpha factor.
- **Returns:** The alpha factor.
<br><br><br><br>

```c++
void SetAlpha(heph_float alpha);
```
Sets the alpha factor.
- **alpha:** The new alpha factor.
