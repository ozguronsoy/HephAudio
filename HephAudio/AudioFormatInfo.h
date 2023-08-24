#pragma once
#include "HephAudioFramework.h"
#include <cstdint>
#if defined(_WIN32)
#include <Mmreg.h>
#include <mmeapi.h>
#endif

#define HEPHAUDIO_INTERNAL_FORMAT(channelCount, sampleRate) AudioFormatInfo(WAVE_FORMAT_IEEE_FLOAT, channelCount, sizeof(heph_float) * 8, sampleRate, HEPH_SYSTEM_ENDIAN)

namespace HephAudio
{
	/// <summary>
	/// Holds information about the audio format.
	/// </summary>
	struct AudioFormatInfo
	{
		/// <summary>
		/// The tag of the audio format.
		/// </summary>
		uint16_t formatTag;
		/// <summary>
		/// The number of channels contained in an audio frame.
		/// </summary>
		uint16_t channelCount;
		/// <summary>
		/// The number of frames rendered or captured in one second, a.k.a sampling frequency, in Hz.
		/// </summary>
		uint32_t sampleRate;
		/// <summary>
		/// The number of bits used to describe an audio sample.
		/// </summary>
		uint16_t bitsPerSample;
		/// <summary>
		/// The endianness of the audio samples.
		/// </summary>
		HephCommon::Endian endian;
		/// <summary>
		/// Creates and initializes an AudioFormatInfo instance with default values.
		/// </summary>
		AudioFormatInfo();
		/// <summary>
		/// Creates and initializes an AudioFormatInfo instance with the provided information.
		/// </summary>
		/// <param name="formatTag">The tag of the audio format.</param>
		/// <param name="nChannels">The number of channels contained in the audio signal.</param>
		/// <param name="bps">The number of bits used to describe an audio sample.</param>
		/// <param name="sampleRate">The number of samples (per channel) rendered or captured in one second, a.k.a sampling frequency, in Hz.</param>
		AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate);
		/// <summary>
		/// Creates and initializes an AudioFormatInfo instance with the provided information.
		/// </summary>
		/// <param name="formatTag">The tag of the audio format.</param>
		/// <param name="nChannels">The number of channels contained in the audio signal.</param>
		/// <param name="bps">The number of bits used to describe an audio sample.</param>
		/// <param name="sampleRate">The number of samples (per channel) rendered or captured in one second, a.k.a sampling frequency, in Hz.</param>
		/// <param name="endian">The endianness of the audio samples.</param>
		AudioFormatInfo(uint16_t formatTag, uint16_t nChannels, uint16_t bps, uint32_t sampleRate, HephCommon::Endian endian);
		/// <summary>
		/// Compares the audio formats.
		/// </summary>
		/// <param name="rhs">The audio format to compare.</param>
		/// <returns>true if the audio formats are the same.</returns>
		bool operator==(const AudioFormatInfo& rhs) const;
		/// <summary>
		/// Compares the audio formats.
		/// </summary>
		/// <param name="rhs">The audio format to compare.</param>
		/// <returns>true if the audio formats are not the same.</returns>
		bool operator!=(const AudioFormatInfo& rhs) const;
		/// <summary>
		/// Calculates the size of an audio frame in bytes.
		/// </summary>
		/// <returns>bitsPerSample * channelCount / 8</returns>
		uint16_t FrameSize() const noexcept;
		/// <summary>
		/// Calculates the number of bits used to represent one second-long audio signal.
		/// </summary>
		/// <returns>sampleRate * bitsPerSample * channelCount</returns>
		uint32_t BitRate() const noexcept;
		/// <summary>
		/// Calculates the number of bytes used to represent one second-long audio signal.
		/// </summary>
		/// <returns>sampleRate * bitsPerSample * channelCount / 8</returns>
		uint32_t ByteRate() const noexcept;
#if defined(_WIN32)
		/// <summary>
		/// Creates and initializes an AudioFormatInfo instance with the provided information.
		/// </summary>
		/// <param name="wfx">The audio format information.</param>
		AudioFormatInfo(const WAVEFORMATEX& wfx);
		/// <summary>
		/// Converts the AudioFormatInfo struct to a WAVEFORMATEX struct.
		/// </summary>
		operator WAVEFORMATEX() const noexcept;
#endif
	};
}