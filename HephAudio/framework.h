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

#define PI 3.14159265358979323846