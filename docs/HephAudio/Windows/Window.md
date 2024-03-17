## Window Class
```c++
#include <Windows/Window.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Methods](#methods)<br>
[Protected Fields](#protected-fields)<br>
[Protected Methods](#protected-methods)

### Description
Base class for windows. Windows are used to reduce artifacts introduced by the Fourier transform. Check this [tutorial](/docs/tutorials/UsingWindows.md) to learn how and when to use windows.<br>
<br><br>

### Methods
```c++
virtual ~Window() = default;
```
Releases the resources.
<br><br><br><br>
```c++
virtual heph_float operator[](size_t n) const = 0;
```
Calculates the sample at index **n**.
- **n:** Index of the sample.
- **Returns:** The sample calculated at the nth index.
<br><br><br><br>
```c++
HephCommon::FloatBuffer GenerateBuffer() const;
```
Generates a buffer.
- **Returns:** The generated buffer.
<br><br><br><br>
```c++
size_t GetSize() const;
```
Gets the size of the window.
- **Returns:** The size of the window.
<br><br><br><br>
```c++
virtual void SetSize(size_t newSize);
```
Sets the size (in frames) of the window.
- **newSize:** The new size of the window.
<br><br><br><br>

### Protected Fields
```c++
class Window
{
	size_t size;
}
```

- ``size``
<br><br>
Size of the window in samples.
<br><br>

### Protected Methods

```c++
Window();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
Window(size_t size);
```
Creates an instance and initializes it with the provided values.
- **size:** Size of the window in samples.
