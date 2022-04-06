#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#endif

#ifdef __ANDROID__
#include <cmath>
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a > b ? b : a)
#endif

#define PI 3.14159265358979323846
#define E_FAIL 0x80004005L
#define E_INVALIDARG 0x80070057L
#define E_NOTIMPL 0x80004001L