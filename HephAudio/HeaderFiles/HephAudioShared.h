#pragma once
#include "HephCommonShared.h"
#include <cmath>

/** @file */

#if !defined(HEPH_ENV_64_BIT)
#error 32-bit is not supported!
#endif

#define HEPHAUDIO_FORMAT_TAG_PCM				(0x0001)
#define HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT			(0x0003)
#define HEPHAUDIO_FORMAT_TAG_ALAW				(0x0006)
#define HEPHAUDIO_FORMAT_TAG_MULAW				(0x0007)
#define HEPHAUDIO_FORMAT_TAG_WMA				(0x0161)
#define HEPHAUDIO_FORMAT_TAG_WMAUDIO_LOSSLESS	(0x0163)
#define HEPHAUDIO_FORMAT_TAG_WMAVOICE			(0x000A)
#define HEPHAUDIO_FORMAT_TAG_MPEG				(0x0050)
#define HEPHAUDIO_FORMAT_TAG_MP3				(0x0055)
#define HEPHAUDIO_FORMAT_TAG_AAC				(0xA106)
#define HEPHAUDIO_FORMAT_TAG_ALAC				(0x6C61)
#define HEPHAUDIO_FORMAT_TAG_FLAC				(0xF1AC)
#define HEPHAUDIO_FORMAT_TAG_OPUS				(0x704F)
#define HEPHAUDIO_FORMAT_TAG_VORBIS				(0x674F)
#define HEPHAUDIO_FORMAT_TAG_EXTENSIBLE			(0xFFFE)

#if defined(HEPHAUDIO_INFO_LOGGING)
#define HEPHAUDIO_LOG(logMessage, logType) HephCommon::ConsoleLogger::Log(logMessage, logType, "HephAudio")
#else
#define HEPHAUDIO_LOG(logMessage, logType)
#endif

/**
 * speed of sound in dry air at 20 deg C in meters per second
 * 
 */
#define HEPHAUDIO_SPEED_OF_SOUND 343.0

#define UINT24_MAX	16777215
#define INT24_MIN	(-8388608)
#define INT24_MAX	8388607

/** @typedef heph_audio_sample_t
 * type of the audio samples, float by default.
 * Can be changed by defining one of the following macros:
 
 * - <b>HEPH_AUDIO_SAMPLE_TYPE_S16</b>
 * - <b>HEPH_AUDIO_SAMPLE_TYPE_S32</b>
 * - <b>HEPH_AUDIO_SAMPLE_TYPE_S64</b>
 * - <b>HEPH_AUDIO_SAMPLE_TYPE_FLT</b>
 * - <b>HEPH_AUDIO_SAMPLE_TYPE_DBL</b>
 * 
 */

/** @def HEPH_AUDIO_SAMPLE_MIN
 * the minimum value an audio sample can have.
 * 
 */

/** @def HEPH_AUDIO_SAMPLE_MAX
 * the maximum value an audio sample can have.
 * 
 */

/** @def HEPH_AUDIO_SAMPLE_TO_IEEE_FLT
 * converts \link heph_audio_sample_t heph_audio_sample_t \endlink to float.
 * 
 */

/** @def HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT
 * converts float to \link heph_audio_sample_t heph_audio_sample_t \endlink.
 * 
 */

#if defined(HEPH_AUDIO_SAMPLE_TYPE_DBL)
typedef double heph_audio_sample_t;
#define HEPH_AUDIO_SAMPLE_MIN					(-1.0)
#define HEPH_AUDIO_SAMPLE_MAX					(1.0)
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#elif defined(HEPH_AUDIO_SAMPLE_TYPE_FLT)
typedef float heph_audio_sample_t;
#define HEPH_AUDIO_SAMPLE_MIN					(-1.0f)
#define HEPH_AUDIO_SAMPLE_MAX					(1.0f)
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#elif defined(HEPH_AUDIO_SAMPLE_TYPE_S64)
typedef int64_t heph_audio_sample_t;
#define HEPH_AUDIO_SAMPLE_MIN					(INT64_MIN)
#define HEPH_AUDIO_SAMPLE_MAX					(INT64_MAX)
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_PCM
#elif defined(HEPH_AUDIO_SAMPLE_TYPE_S32)
typedef int32_t heph_audio_sample_t;
#define HEPH_AUDIO_SAMPLE_MIN					(INT32_MIN)
#define HEPH_AUDIO_SAMPLE_MAX					(INT32_MAX)
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_PCM
#elif defined(HEPH_AUDIO_SAMPLE_TYPE_S16)
typedef int16_t heph_audio_sample_t;
#define HEPH_AUDIO_SAMPLE_MIN					(INT16_MIN)
#define HEPH_AUDIO_SAMPLE_MAX					(INT16_MAX)
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_PCM
#else
typedef float heph_audio_sample_t;
#define HEPH_AUDIO_SAMPLE_MIN					(-1.0f)
#define HEPH_AUDIO_SAMPLE_MAX					(1.0f)
#define HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT
#endif

#if HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL == HEPHAUDIO_FORMAT_TAG_PCM

#define HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(sample)		(((double)sample) / (-((double)HEPH_AUDIO_SAMPLE_MIN)))
#define HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(fltSample)	((heph_audio_sample_t)((fltSample) * (-((double)HEPH_AUDIO_SAMPLE_MIN))))

#elif HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL == HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT

#define HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(sample) 		(sample)
#define HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(fltSample) 	(fltSample)

#else
#error heph_audio_sample_t/ieee_float conversions are not implemented for this internal format yet.
#endif

/**
 * the minimum API level required for OPENSL ES.
 * 
 */
#define HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL (21)

/**
 * the minimum API level required for AAudio.
 * 
 */
#define HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL (26)

namespace HephAudio
{
	/**
	 * converts decibel to gain (between -1 and 1).
	 * 
	 */
	inline double DecibelToGain(double decibel)
	{
		return pow(10.0, decibel * 0.05);
	}

	/**
	 * converts gain (between -1 and 1) to decibel.
	 * 
	 */
	inline double GainToDecibel(double gain)
	{
		return gain == 0 ? -120.0 : 20.0 * log10(abs(gain));
	}

	/**
	 * converts semitone to cent.
	 * 
	 */
	inline constexpr double SemitoneToCent(double semitone)
	{
		return semitone * 100.0;
	}

	/**
	 * converts semitone to octave.
	 * 
	 */
	inline constexpr double SemitoneToOctave(double semitone)
	{
		return semitone / 12.0;
	}

	/**
	 * converts cent to semitone.
	 * 
	 */
	inline constexpr double CentToSemitone(double cent)
	{
		return cent / 100.0;
	}

	/**
	 * converts cent to octave.
	 * 
	 */
	inline constexpr double CentToOctave(double cent)
	{
		return cent / 1200.0;
	}

	/**
	 * converts octave to semitone.
	 * 
	 */
	inline constexpr double OctaveToSemitone(double octave)
	{
		return octave * 12.0;
	}

	/**
	 * converts octave to cent.
	 * 
	 */
	inline constexpr double OctaveToCent(double octave)
	{
		return octave * 1200.0;
	}
}