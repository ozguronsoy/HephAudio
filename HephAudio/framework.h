#pragma once
#ifdef _WIN32
#ifdef HEPHAUDIODLL_EXPORTS
#define HephAudioAPI __declspec(dllexport)
#else
#define HephAudioAPI __declspec(dllimport)
#endif // HEPHAUDIO_EXPORTS

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#else
#define HephAudioAPI
#endif
#ifdef __ANDROID__
#include <cmath>
#endif

#define PI 3.14159265358979323846
#define E_FAIL 0x80004005L
#define E_INVALIDARG 0x80070057L
#define E_NOTIMPL 0x80004001L