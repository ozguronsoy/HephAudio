#pragma once
#include "HephAudioShared.h"
#include <string>
#include <cinttypes>

/** @file */

namespace HephAudio
{
	enum AudioDeviceType
	{
		Null = 0,

		/**
		 * indicates the audio device is capable of rendering (playing) audio data.
		 * 
		 */
		Render = 1,

		/**
		 * indicates the device is capable of capturing (recording) audio data.
		 * 
		 */
		Capture = 2,

		/**
		 * indicates the device is capable of both rendering and capturing audio data.
		 * 
		 */
		All = Render | Capture
	};

	constexpr AudioDeviceType operator|(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs | (uint8_t)rhs);
	}

	constexpr AudioDeviceType& operator|=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}

	constexpr AudioDeviceType operator&(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs & (uint8_t)rhs);
	}

	constexpr AudioDeviceType& operator&=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		lhs = lhs & rhs;
		return lhs;
	}

	constexpr AudioDeviceType operator^(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs ^ (uint8_t)rhs);
	}

	constexpr AudioDeviceType& operator^=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		lhs = lhs ^ rhs;
		return lhs;
	}

	constexpr AudioDeviceType operator~(const AudioDeviceType& lhs)
	{
		return (AudioDeviceType)(~(uint8_t)lhs);
	}

	/**
	 * @brief stores information about an audio device.
	 * 
	 */
	struct HEPH_API AudioDevice
	{
		/**
		 * unique identifier of the audio device provided by the native API.
		 * 
		 */
		std::string id;

		/**
		 * name of the audio device provided by the native API.
		 * 
		 */
		std::string name;

		/**
		 * auido device type.
		 * 
		 */
		AudioDeviceType type;

		/**
		 * indicates whether the audio device is the system default.
		 * 
		 */
		bool isDefault;

		/** @copydoc default_constructor */
		AudioDevice();
	};
}