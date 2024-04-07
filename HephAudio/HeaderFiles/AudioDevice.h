#pragma once
#include "HephAudioShared.h"
#include <string>
#include <cinttypes>

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

	struct AudioDevice
	{
		std::string id;
		std::string name;
		AudioDeviceType type;
		bool isDefault;
		AudioDevice() : id(""), name(""), type(AudioDeviceType::Render), isDefault(false) {}
	};
}