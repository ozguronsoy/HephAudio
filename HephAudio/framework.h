#pragma once
#include "HephCommonFramework.h"
#include <cmath>

#ifndef HEPHAUDIO_FLOAT

#ifdef HEPHAUDIO_HIGH_PRECISION
typedef double hephaudio_float;
#else
typedef float hephaudio_float;
#endif

#define HEPHAUDIO_FLOAT hephaudio_float
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#endif

#ifdef __ANDROID__
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) > (b)) ? (b) : (a))
#define WAVE_FORMAT_PCM 1
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

#pragma region Helper Methods
inline constexpr hephaudio_float sgn(hephaudio_float x)
{
	if (x > 0)
	{
		return 1.0;
	}
	else if (x < 0)
	{
		return -1.0;
	}
	return 0.0;
}
inline hephaudio_float DecibelToGain(hephaudio_float decibel)
{
	return pow(10.0, decibel * 0.05);
}
inline hephaudio_float GainToDecibel(hephaudio_float gain)
{
	return gain == 0 ? -120.0 : 20.0 * log10(abs(gain));
}
inline constexpr hephaudio_float RadToDeg(hephaudio_float rad)
{
	return rad * 180.0 / PI;
}
inline constexpr hephaudio_float DegToRad(hephaudio_float deg)
{
	return deg * PI / 180.0;
}
inline constexpr hephaudio_float SemitoneToCent(hephaudio_float semitone)
{
	return semitone * 100.0;
}
inline constexpr hephaudio_float SemitoneToOctave(hephaudio_float semitone)
{
	return semitone / 12.0;
}
inline constexpr hephaudio_float CentToSemitone(hephaudio_float cent)
{
	return cent / 100.0;
}
inline constexpr hephaudio_float CentToOctave(hephaudio_float cent)
{
	return cent / 1200.0;
}
inline constexpr hephaudio_float OctaveToSemitone(hephaudio_float octave)
{
	return octave * 12.0;
}
inline constexpr hephaudio_float OctaveToCent(hephaudio_float octave)
{
	return octave * 1200.0;
}
#pragma endregion