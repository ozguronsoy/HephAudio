#pragma once
#include "HephAudioShared.h"
#include <cstdint>
#include <vector>

/** @file */

#define HEPHAUDIO_CH_MASK_MONO (HephAudio::AudioChannelMask::FrontCenter)
#define HEPHAUDIO_CH_MASK_STEREO (HephAudio::AudioChannelMask::FrontLeft | HephAudio::AudioChannelMask::FrontRight)
#define HEPHAUDIO_CH_MASK_2_POINT_1 (HEPHAUDIO_CH_MASK_STEREO | HephAudio::AudioChannelMask::LowFrequency)
#define HEPHAUDIO_CH_MASK_2_1 (HEPHAUDIO_CH_MASK_STEREO | HephAudio::AudioChannelMask::BackCenter)
#define HEPHAUDIO_CH_MASK_SURROUND (HEPHAUDIO_CH_MASK_STEREO | HephAudio::AudioChannelMask::FrontCenter)
#define HEPHAUDIO_CH_MASK_3_POINT_1 (HEPHAUDIO_CH_MASK_SURROUND | HephAudio::AudioChannelMask::LowFrequency)
#define HEPHAUDIO_CH_MASK_4_POINT_0 (HEPHAUDIO_CH_MASK_SURROUND | HephAudio::AudioChannelMask::BackCenter)
#define HEPHAUDIO_CH_MASK_2_2 (HEPHAUDIO_CH_MASK_STEREO | HephAudio::AudioChannelMask::SideLeft | HephAudio::AudioChannelMask::SideRight)
#define HEPHAUDIO_CH_MASK_QUAD (HEPHAUDIO_CH_MASK_STEREO | HephAudio::AudioChannelMask::BackLeft | HephAudio::AudioChannelMask::BackRight)
#define HEPHAUDIO_CH_MASK_4_POINT_1 (HEPHAUDIO_CH_MASK_4_POINT_0 | HephAudio::AudioChannelMask::LowFrequency)
#define HEPHAUDIO_CH_MASK_5_POINT_0 (HEPHAUDIO_CH_MASK_SURROUND | HephAudio::AudioChannelMask::SideLeft | HephAudio::AudioChannelMask::SideRight)
#define HEPHAUDIO_CH_MASK_5_POINT_0_BACK (HEPHAUDIO_CH_MASK_SURROUND | HephAudio::AudioChannelMask::BackLeft | HephAudio::AudioChannelMask::BackRight)
#define HEPHAUDIO_CH_MASK_5_POINT_1 (HEPHAUDIO_CH_MASK_5_POINT_0 | HephAudio::AudioChannelMask::LowFrequency)
#define HEPHAUDIO_CH_MASK_5_POINT_1_BACK (HEPHAUDIO_CH_MASK_5_POINT_0_BACK | HephAudio::AudioChannelMask::LowFrequency)
#define HEPHAUDIO_CH_MASK_6_POINT_0 (HEPHAUDIO_CH_MASK_5_POINT_0 | HephAudio::AudioChannelMask::BackCenter)
#define HEPHAUDIO_CH_MASK_6_POINT_0_FRONT (HEPHAUDIO_CH_MASK_2_2 | HephAudio::AudioChannelMask::FrontLeftOfCenter | HephAudio::AudioChannelMask::FrontRightOfCenter)
#define HEPHAUDIO_CH_MASK_HEXAGONAL (HEPHAUDIO_CH_MASK_5_POINT_0_BACK | HephAudio::AudioChannelMask::BackCenter)
#define HEPHAUDIO_CH_MASK_6_POINT_1 (HEPHAUDIO_CH_MASK_5_POINT_1 | HephAudio::AudioChannelMask::BackCenter)
#define HEPHAUDIO_CH_MASK_6_POINT_1_BACK (HEPHAUDIO_CH_MASK_5_POINT_1_BACK | HephAudio::AudioChannelMask::BackCenter)
#define HEPHAUDIO_CH_MASK_6_POINT_1_FRONT (HEPHAUDIO_CH_MASK_6_POINT_0_FRONT | HephAudio::AudioChannelMask::LowFrequency)
#define HEPHAUDIO_CH_MASK_7_POINT_0 (HEPHAUDIO_CH_MASK_5_POINT_0 | HephAudio::AudioChannelMask::BackLeft | HephAudio::AudioChannelMask::BackRight)
#define HEPHAUDIO_CH_MASK_7_POINT_0_FRONT (HEPHAUDIO_CH_MASK_5_POINT_0 | HephAudio::AudioChannelMask::FrontLeftOfCenter | HephAudio::AudioChannelMask::FrontRightOfCenter)
#define HEPHAUDIO_CH_MASK_7_POINT_1 (HEPHAUDIO_CH_MASK_5_POINT_1 | HephAudio::AudioChannelMask::BackLeft | HephAudio::AudioChannelMask::BackRight)
#define HEPHAUDIO_CH_MASK_7_POINT_1_WIDE (HEPHAUDIO_CH_MASK_5_POINT_1 | HephAudio::AudioChannelMask::FrontLeftOfCenter | HephAudio::AudioChannelMask::FrontRightOfCenter)
#define HEPHAUDIO_CH_MASK_7_POINT_1_WIDE_BACK (HEPHAUDIO_CH_MASK_5_POINT_1_BACK | HephAudio::AudioChannelMask::FrontLeftOfCenter | HephAudio::AudioChannelMask::FrontRightOfCenter)
#define HEPHAUDIO_CH_MASK_OCTAGONAL (HEPHAUDIO_CH_MASK_5_POINT_0 | HephAudio::AudioChannelMask::BackLeft | HephAudio::AudioChannelMask::BackCenter | HephAudio::AudioChannelMask::BackRight)

#define HEPHAUDIO_CH_LAYOUT_MONO HephAudio::AudioChannelLayout(1, HEPHAUDIO_CH_MASK_MONO)
#define HEPHAUDIO_CH_LAYOUT_STEREO HephAudio::AudioChannelLayout(2, HEPHAUDIO_CH_MASK_STEREO)
#define HEPHAUDIO_CH_LAYOUT_2_POINT_1 HephAudio::AudioChannelLayout(3, HEPHAUDIO_CH_MASK_2_POINT_1)
#define HEPHAUDIO_CH_LAYOUT_2_1 HephAudio::AudioChannelLayout(3, HEPHAUDIO_CH_MASK_2_1)
#define HEPHAUDIO_CH_LAYOUT_SURROUND HephAudio::AudioChannelLayout(3, HEPHAUDIO_CH_MASK_SURROUND)
#define HEPHAUDIO_CH_LAYOUT_3_POINT_1 HephAudio::AudioChannelLayout(4, HEPHAUDIO_CH_MASK_3_POINT_1)
#define HEPHAUDIO_CH_LAYOUT_4_POINT_0 HephAudio::AudioChannelLayout(4, HEPHAUDIO_CH_MASK_4_POINT_0)
#define HEPHAUDIO_CH_LAYOUT_2_2 HephAudio::AudioChannelLayout(4, HEPHAUDIO_CH_MASK_2_2)
#define HEPHAUDIO_CH_LAYOUT_QUAD HephAudio::AudioChannelLayout(4, HEPHAUDIO_CH_MASK_QUAD)
#define HEPHAUDIO_CH_LAYOUT_4_POINT_1 HephAudio::AudioChannelLayout(5, HEPHAUDIO_CH_MASK_4_POINT_1)
#define HEPHAUDIO_CH_LAYOUT_5_POINT_0 HephAudio::AudioChannelLayout(5, HEPHAUDIO_CH_MASK_5_POINT_0)
#define HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK HephAudio::AudioChannelLayout(5, HEPHAUDIO_CH_MASK_5_POINT_0_BACK)
#define HEPHAUDIO_CH_LAYOUT_5_POINT_1 HephAudio::AudioChannelLayout(6, HEPHAUDIO_CH_MASK_5_POINT_1)
#define HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK HephAudio::AudioChannelLayout(6, HEPHAUDIO_CH_MASK_5_POINT_1_BACK)
#define HEPHAUDIO_CH_LAYOUT_6_POINT_0 HephAudio::AudioChannelLayout(6, HEPHAUDIO_CH_MASK_6_POINT_0)
#define HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT HephAudio::AudioChannelLayout(6, HEPHAUDIO_CH_MASK_6_POINT_0_FRONT)
#define HEPHAUDIO_CH_LAYOUT_HEXAGONAL HephAudio::AudioChannelLayout(6, HEPHAUDIO_CH_MASK_HEXAGONAL)
#define HEPHAUDIO_CH_LAYOUT_6_POINT_1 HephAudio::AudioChannelLayout(7, HEPHAUDIO_CH_MASK_6_POINT_1)
#define HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK HephAudio::AudioChannelLayout(7, HEPHAUDIO_CH_MASK_6_POINT_1_BACK)
#define HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT HephAudio::AudioChannelLayout(7, HEPHAUDIO_CH_MASK_6_POINT_1_FRONT)
#define HEPHAUDIO_CH_LAYOUT_7_POINT_0 HephAudio::AudioChannelLayout(7, HEPHAUDIO_CH_MASK_7_POINT_0)
#define HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT HephAudio::AudioChannelLayout(7, HEPHAUDIO_CH_MASK_7_POINT_0_FRONT)
#define HEPHAUDIO_CH_LAYOUT_7_POINT_1 HephAudio::AudioChannelLayout(8, HEPHAUDIO_CH_MASK_7_POINT_1)
#define HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE HephAudio::AudioChannelLayout(8, HEPHAUDIO_CH_MASK_7_POINT_1_WIDE)
#define HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK HephAudio::AudioChannelLayout(8, HEPHAUDIO_CH_MASK_7_POINT_1_WIDE_BACK)
#define HEPHAUDIO_CH_LAYOUT_OCTAGONAL HephAudio::AudioChannelLayout(8, HEPHAUDIO_CH_MASK_OCTAGONAL)


namespace HephAudio
{
	/**
	 * bit field that indicates which channels are used.
	 * 
	 */
	enum class AudioChannelMask : uint32_t
	{
		Unknown = 0,
		FrontLeft = 0x1,
		FrontRight = 0x2,
		FrontCenter = 0x4,
		LowFrequency = 0x8,
		BackLeft = 0x10,
		BackRight = 0x20,
		FrontLeftOfCenter = 0x40,
		FrontRightOfCenter = 0x80,
		BackCenter = 0x100,
		SideLeft = 0x200,
		SideRight = 0x400,
		TopCenter = 0x800,
		TopFrontLeft = 0x1000,
		TopFrontCenter = 0x2000,
		TopFrontRight = 0x4000,
		TopBackLeft = 0x8000,
		TopBackCenter = 0x10000,
		TopBackRight = 0x20000
	};


	constexpr AudioChannelMask operator|(const AudioChannelMask& lhs, const AudioChannelMask& rhs)
	{
		return (AudioChannelMask)(((int)lhs) | ((int)rhs));
	}
	
	constexpr AudioChannelMask& operator|=(AudioChannelMask& lhs, const AudioChannelMask& rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}
	
	constexpr AudioChannelMask operator&(const AudioChannelMask& lhs, const AudioChannelMask& rhs)
	{
		return (AudioChannelMask)(((int)lhs) & ((int)rhs));
	}
	
	constexpr AudioChannelMask& operator&=(AudioChannelMask& lhs, const AudioChannelMask& rhs)
	{
		lhs = lhs & rhs;
		return lhs;
	}
	
	constexpr AudioChannelMask operator^(const AudioChannelMask& lhs, const AudioChannelMask& rhs)
	{
		return (AudioChannelMask)(((int)lhs) ^ ((int)rhs));
	}
	
	constexpr AudioChannelMask& operator^=(AudioChannelMask& lhs, const AudioChannelMask& rhs)
	{
		lhs = lhs ^ rhs;
		return lhs;
	}
	
	constexpr AudioChannelMask operator~(const AudioChannelMask& lhs)
	{
		return (AudioChannelMask)(~((int)lhs));
	}

	/**
	 * @brief stores information about the channel layout.
	 * 
	 */
	struct AudioChannelLayout
	{
		/**
		 * number of channels present.
		 * 
		 */
		uint16_t count;

		/**
		 * indicates which channels are used.
		 * 
		 */
		AudioChannelMask mask;

		constexpr AudioChannelLayout() : AudioChannelLayout(0, AudioChannelMask::Unknown) {}
		
		constexpr AudioChannelLayout(uint16_t count, AudioChannelMask mask) : count(count), mask(mask) {}
		
		constexpr bool operator==(const AudioChannelLayout& rhs) const
		{
			return this->count == rhs.count && this->mask == rhs.mask;
		}
		
		constexpr bool operator!=(const AudioChannelLayout& rhs) const
		{
			return this->count != rhs.count || this->mask != rhs.mask;
		}
		
		/**
		 * gets the default channel mask for the \a channelCount.
		 * 
		 */
		static constexpr AudioChannelMask DefaultChannelMask(uint16_t channelCount)
		{
			switch (channelCount)
			{
			case 1:
				return HEPHAUDIO_CH_MASK_MONO;
			case 2:
				return HEPHAUDIO_CH_MASK_STEREO;
			case 3:
				return HEPHAUDIO_CH_MASK_SURROUND;
			case 4:
				return HEPHAUDIO_CH_MASK_4_POINT_0;
			case 5:
				return HEPHAUDIO_CH_MASK_5_POINT_0;
			case 6:
				return HEPHAUDIO_CH_MASK_5_POINT_1;
			case 7:
				return HEPHAUDIO_CH_MASK_7_POINT_0;
			case 8:
				return HEPHAUDIO_CH_MASK_7_POINT_1;
			default:
				return AudioChannelMask::Unknown;
			}
		}
	
		/**
		 * gets the default channel layout for the \a channelCount.
		 * 
		 */
		static constexpr AudioChannelLayout DefaultChannelLayout(uint16_t channelCount)
		{
			return AudioChannelLayout(channelCount, AudioChannelLayout::DefaultChannelMask(channelCount));
		}
		
		/**
		 * gets the number of channels used.
		 * 
		 */
		static constexpr uint16_t GetChannelCount(AudioChannelMask mask)
		{
			uint16_t channelCount = 0;
			uint32_t mask32 = (uint32_t)mask;
			for (size_t i = 0; i < 32; i++)
			{
				if (mask32 & 1)
				{
					channelCount++;
				}
				mask32 >>= 1;
			}
			return channelCount;
		}
		
		/**
		 * gets the number of channels used in the layout.
		 * 
		 */
		static constexpr uint16_t GetChannelCount(const AudioChannelLayout& layout)
		{
			return AudioChannelLayout::GetChannelCount(layout.mask);
		}
		
		/**
		 * gets the channel mapping that corresponds to the channel mask.
		 * 
		 */
		static std::vector<AudioChannelMask> GetChannelMapping(AudioChannelMask mask);

		/**
		 * gets the channel mapping that corresponds to the channel layout.
		 * 
		 */
		static std::vector<AudioChannelMask> GetChannelMapping(const AudioChannelLayout& layout);
	};
}