## HannPoissonWindow Class
```c++
#include <Windows/HannPoissonWindow.h>
using namespace HephAudio;
```
**Inheritance:** *[Window](/docs/HephAudio/Windows/Window.md)* -> *HannPoissonWindow*

### Fields
```c++
class HannPoissonWindow final : public Window
{
    heph_float alpha;
};

```
- ``alpha``
<br><br>
Controls the slope.
<br><br>

### Methods
```c++
HannPoissonWindow();
```
Creates an instance and initializes it with default values.
<br><br><br><br>
```c++
HannPoissonWindow(size_t size);
```
Creates an instance and initializes it with provided values.
- **size:** Size of the window in frames.
<br><br><br><br>
```c++
HannPoissonWindow(size_t size, heph_float alpha);
```
Creates an instance and initializes it with provided values.
- **size:** Size of the window in frames.
- **alpha:** Controls the slope.
<br><br><br><br>
