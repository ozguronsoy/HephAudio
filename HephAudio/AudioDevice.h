#pragma once
#include "framework.h"
#include <string>

namespace HephAudio
{
	namespace Structs
	{
		enum class HephAudioAPI AudioDeviceType : uint8_t
		{
			Null = 0,
			Render = 1,
			Capture = 2,
			All = Render | Capture
		};
		struct HephAudioAPI AudioDevice
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
		constexpr HephAudioAPI AudioDeviceType operator|(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
		{
			return (AudioDeviceType)((uint8_t)lhs | (uint8_t)rhs);
		}
		constexpr HephAudioAPI AudioDeviceType& operator|=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
		{
			lhs = lhs | rhs;
			return lhs;
		}
		constexpr HephAudioAPI AudioDeviceType operator&(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
		{
			return (AudioDeviceType)((uint8_t)lhs & (uint8_t)rhs);
		}
		constexpr HephAudioAPI AudioDeviceType& operator&=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
		{
			lhs = lhs & rhs;
			return lhs;
		}
		constexpr HephAudioAPI AudioDeviceType operator^(const AudioDeviceType& lhs, const AudioDeviceType& rhs)
		{
			return (AudioDeviceType)((uint8_t)lhs ^ (uint8_t)rhs);
		}
		constexpr HephAudioAPI AudioDeviceType& operator^=(AudioDeviceType& lhs, const AudioDeviceType& rhs)
		{
			lhs = lhs ^ rhs;
			return lhs;
		}
		constexpr HephAudioAPI AudioDeviceType operator~(const AudioDeviceType& lhs)
		{
			return (AudioDeviceType)(~(uint8_t)lhs);
		}
	}
}