#pragma once
#include "HephAudioShared.h"
#include "AudioChannelLayout.h"

/** @file */

/**
 * creates an AudioFormatInfo instance with internal format.
 * 
 */
#define HEPHAUDIO_INTERNAL_FORMAT(chLayout, sampleRate) HephAudio::AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_HEPHAUDIO_INTERNAL, sizeof(heph_audio_sample_t) * 8, chLayout, sampleRate, HEPH_SYSTEM_ENDIAN)

namespace HephAudio
{
	/**
	 * @brief stores the properties of the audio signals
	 * 
	 */
	struct AudioFormatInfo
	{
		/**
		 * tag of the format that audio data is encoded with.
		 * 
		 */
		uint16_t formatTag;

		/**
		 * bit depth, the number of bits required to represent a sample.
		 * 
		 */
		uint16_t bitsPerSample;

		/** @copydoc AudioChannelLayout */
		AudioChannelLayout channelLayout;

		/**
		 * samples per second, the number of samples required to represent a one-second-long audio signal.
		 * 
		 */
		uint32_t sampleRate;

		/**
		 * bits per second, the number of bits required to represent a one-second-long audio signal.
		 * 
		 */
		uint32_t bitRate;

		/**
		 * endianness of the audio samples.
		 * 
		 */
		HephCommon::Endian endian;

		/** @copydoc default_constructor */
		AudioFormatInfo();

		/**
		 * @copydoc constructor
		 * 
		 * @param formatTag @copydetails formatTag
		 * @param bitsPerSample @copydetails bitsPerSample
		 * @param chLayout @copybrief channelLayout
		 * @param sampleRate @copydetails sampleRate
		 */
		AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate);

		/**
		 * @copydoc constructor
		 * 
		 * @param formatTag @copydetails formatTag
		 * @param bitsPerSample @copydetails bitsPerSample
		 * @param chLayout @copybrief channelLayout
		 * @param sampleRate @copydetails sampleRate
		 * @param bitRate @copydetails bitRate
		 */
		AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, uint32_t bitRate);

		/**
		 * @copydoc constructor
		 * 
		 * @param formatTag @copydetails formatTag
		 * @param bitsPerSample @copydetails bitsPerSample
		 * @param chLayout @copybrief channelLayout
		 * @param sampleRate @copydetails sampleRate
		 * @param endian @copydetails endian
		 */
		AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, HephCommon::Endian endian);
		
		/**
		 * @copydoc constructor
		 * 
		 * @param formatTag @copydetails formatTag
		 * @param bitsPerSample @copydetails bitsPerSample
		 * @param chLayout @copybrief channelLayout
		 * @param sampleRate @copydetails sampleRate
		 * @param bitRate @copydetails bitRate
		 * @param endian @copydetails endian
		 */
		AudioFormatInfo(uint16_t formatTag, uint16_t bitsPerSample, AudioChannelLayout chLayout, uint32_t sampleRate, uint32_t bitRate, HephCommon::Endian endian);
		
		bool operator==(const AudioFormatInfo& rhs) const;
		bool operator!=(const AudioFormatInfo& rhs) const;
		
		/**
		 * calculates the size of an audio frame in bytes.
		 *  
		 */
		uint16_t FrameSize() const;
		
		/**
		 * calculates the number of bytes required to represent a one-second-long audio signal.
		 * 
		 */
		uint32_t ByteRate() const;
		
		/**
		 * calculates the bitrate of the PCM, IEEE_FLOAT, ALAW, and MULAW codecs.
		 * 
		 */
		static uint32_t CalculateBitrate(const AudioFormatInfo& formatInfo);
	};
}