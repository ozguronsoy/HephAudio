#pragma once
#include "framework.h"
#include <string>

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
		std::wstring id;
		std::wstring name;
		std::wstring description;
		AudioDeviceType type;
		bool isDefault;
		AudioDevice()
		{
			id = L"";
			name = L"";
			description = L"";
			type = AudioDeviceType::Render;
			isDefault = false;
		}
		virtual ~AudioDevice() = default;
	};
	constexpr AudioDeviceType operator|(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs | (uint8_t)rhs);
	}
	constexpr AudioDeviceType operator|=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs | rhs;
	}
	constexpr AudioDeviceType operator&(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs & (uint8_t)rhs);
	}
	constexpr AudioDeviceType operator&=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs & rhs;
	}
	constexpr AudioDeviceType operator^(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return (AudioDeviceType)((uint8_t)lhs ^ (uint8_t)rhs);
	}
	constexpr AudioDeviceType operator^=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
	{
		return lhs ^ rhs;
	}
	constexpr AudioDeviceType operator~(const AudioDeviceType& lhs)
	{
		return (AudioDeviceType)(~(uint8_t)lhs);
	}
}