#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#endif

#ifdef __ANDROID__
#include <cmath>
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) > (b)) ? (b) : (a))
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif
#ifndef E_FAIL
#define E_FAIL 0x80004005L
#endif
#ifndef E_INVALIDARG
#define E_INVALIDARG 0x80070057L
#endif
#ifndef E_NOTIMPL
#define E_NOTIMPL 0x80004001L
#endif
#ifndef E_OUTOFMEMORY
#define E_OUTOFMEMORY 0x8007000E
#endif