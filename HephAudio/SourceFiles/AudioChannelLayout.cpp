#include "AudioChannelLayout.h"

namespace HephAudio
{
	std::vector<AudioChannelMask> AudioChannelLayout::GetChannelMapping(AudioChannelMask mask)
	{
		switch (mask)
		{
		case HEPHAUDIO_CH_MASK_MONO:
			return { AudioChannelMask::FrontCenter };
		case HEPHAUDIO_CH_MASK_STEREO:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight };
		case HEPHAUDIO_CH_MASK_2_POINT_1:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::LowFrequency };
		case HEPHAUDIO_CH_MASK_2_1:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::BackCenter };
		case HEPHAUDIO_CH_MASK_SURROUND:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter };
		case HEPHAUDIO_CH_MASK_3_POINT_1:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency };
		case HEPHAUDIO_CH_MASK_4_POINT_0:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::BackCenter };
		case HEPHAUDIO_CH_MASK_2_2:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_QUAD:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::BackLeft, AudioChannelMask::BackRight };
		case HEPHAUDIO_CH_MASK_4_POINT_1:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency, AudioChannelMask::BackCenter };
		case HEPHAUDIO_CH_MASK_5_POINT_0:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_5_POINT_0_BACK:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::BackLeft, AudioChannelMask::BackRight };
		case HEPHAUDIO_CH_MASK_5_POINT_1:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_5_POINT_1_BACK:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency, AudioChannelMask::BackLeft, AudioChannelMask::BackRight };
		case HEPHAUDIO_CH_MASK_6_POINT_0:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::BackCenter, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_6_POINT_0_FRONT:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontLeftOfCenter, AudioChannelMask::FrontRightOfCenter, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_HEXAGONAL:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::BackLeft, AudioChannelMask::BackRight, AudioChannelMask::BackCenter };
		case HEPHAUDIO_CH_MASK_6_POINT_1:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency, AudioChannelMask::BackCenter, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_6_POINT_1_BACK:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency, AudioChannelMask::BackLeft, AudioChannelMask::BackRight, AudioChannelMask::BackCenter };
		case HEPHAUDIO_CH_MASK_6_POINT_1_FRONT:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::LowFrequency, AudioChannelMask::FrontLeftOfCenter, AudioChannelMask::FrontRightOfCenter, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_7_POINT_0:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::BackLeft, AudioChannelMask::BackRight, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_7_POINT_0_FRONT:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::FrontLeftOfCenter, AudioChannelMask::FrontRightOfCenter, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_7_POINT_1:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency, AudioChannelMask::BackLeft, AudioChannelMask::BackRight, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_7_POINT_1_WIDE:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency, AudioChannelMask::FrontLeftOfCenter, AudioChannelMask::FrontRightOfCenter, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		case HEPHAUDIO_CH_MASK_7_POINT_1_WIDE_BACK:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::LowFrequency, AudioChannelMask::BackLeft, AudioChannelMask::BackRight, AudioChannelMask::FrontLeftOfCenter, AudioChannelMask::FrontRightOfCenter };
		case HEPHAUDIO_CH_MASK_OCTAGONAL:
			return { AudioChannelMask::FrontLeft, AudioChannelMask::FrontRight, AudioChannelMask::FrontCenter, AudioChannelMask::BackLeft, AudioChannelMask::BackRight, AudioChannelMask::BackCenter, AudioChannelMask::SideLeft, AudioChannelMask::SideRight };
		default:
			return {};
		}
	}
	std::vector<AudioChannelMask> AudioChannelLayout::GetChannelMapping(const AudioChannelLayout& layout)
	{
		return AudioChannelLayout::GetChannelMapping(layout.mask);
	}
}