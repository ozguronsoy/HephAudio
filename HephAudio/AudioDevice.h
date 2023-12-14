#pragma once
#include "HephAudioFramework.h"
#include "../HephCommon/HeaderFiles/StringBuffer.h"
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
	struct AudioDevice
	{
		HephCommon::StringBuffer id{ "" };
		HephCommon::StringBuffer name{ "" };
		AudioDeviceType type{ AudioDeviceType::Render };
		bool isDefault{ false };
	};
	HEPH_CONSTEXPR AudioDeviceType operator|(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs | (uint8_t)rhs);
	}
	HEPH_CONSTEXPR AudioDeviceType operator|=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs | rhs;
	}
	HEPH_CONSTEXPR AudioDeviceType operator&(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs & (uint8_t)rhs);
	}
	HEPH_CONSTEXPR AudioDeviceType operator&=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs & rhs;
	}
	HEPH_CONSTEXPR AudioDeviceType operator^(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs ^ (uint8_t)rhs);
	}
	HEPH_CONSTEXPR AudioDeviceType operator^=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs ^ rhs;
	}
	HEPH_CONSTEXPR AudioDeviceType operator~(const AudioDeviceType& lhs)
	{
		return (AudioDeviceType)(~(uint8_t)lhs);
	}
}