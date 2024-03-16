## EqualizerInfo Struct
```c++
#include <EqualizerInfo.h>
using namespace HephAudio;
```

### Description
Container for equalizer effect parameters.
<br><br>

### Parameters
```c++
struct EqualizerInfo
{
    heph_float f1;
    heph_float f2;
    heph_float amplitude;
};
```

- ``f1``
<br><br>
First frequency in Hz.
<br><br>

- ``f2``
<br><br>
Second frequency in Hz.
<br><br>

- ``amplitude``
<br><br>
Multiplication factor of the frequency components between ``f1`` and ``f2``.
<br><br>

### Methods

```c++
EqualizerInfo();
```
Creates an instance and initializes it with default values.
<br><br><br><br>

```c++
EqualizerInfo(heph_float f1,
              heph_float f2,
              heph_float amplitude);
```
Creates an instance and initializes it with the provided values.
