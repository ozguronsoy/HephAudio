#pragma once
#include "HephAudioShared.h"
#include <string>
#include <cinttypes>

/** @file */

namespace HephAudio
{
	enum class AudioDeviceType : uint8_t
	{
		Null = 0,
		Render = 1,
		Capture = 2,
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
	struct AudioDevice
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

		AudioDevice() : id(""), name(""), type(AudioDeviceType::Render), isDefault(false) {}
	};
}