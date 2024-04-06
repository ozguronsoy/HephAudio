#include "AudioChannelMixingLookupTables.h"

namespace HephAudio
{
	hephaudio_channel_mixing_lookup_table_t AudioChannelMixingLookupTables::_mono_table =
	{
		{
			HephAudio::AudioChannelMask::FrontCenter,
			{
				{ HephAudio::AudioChannelMask::FrontLeft, 1 },
				{ HephAudio::AudioChannelMask::FrontRight, 1 },
				{ HephAudio::AudioChannelMask::FrontCenter, 1 },
				{ HephAudio::AudioChannelMask::LowFrequency, 1 },
				{ HephAudio::AudioChannelMask::FrontLeftOfCenter, 1 },
				{ HephAudio::AudioChannelMask::FrontRightOfCenter, 1 },
				{ HephAudio::AudioChannelMask::SideLeft, 1 },
				{ HephAudio::AudioChannelMask::SideRight, 1 },
				{ HephAudio::AudioChannelMask::BackLeft, 1 },
				{ HephAudio::AudioChannelMask::BackRight, 1 },
				{ HephAudio::AudioChannelMask::BackCenter, 1 },
				{ HephAudio::AudioChannelMask::TopCenter, 1 },
				{ HephAudio::AudioChannelMask::TopFrontLeft, 1 },
				{ HephAudio::AudioChannelMask::TopFrontRight, 1 },
				{ HephAudio::AudioChannelMask::TopFrontCenter, 1 },
				{ HephAudio::AudioChannelMask::TopBackLeft, 1 },
				{ HephAudio::AudioChannelMask::TopBackRight, 1 },
				{ HephAudio::AudioChannelMask::TopBackCenter, 1 }
			}
		}
	};

	hephaudio_channel_mixing_lookup_table_t AudioChannelMixingLookupTables::_stereo_table =
	{
		{
			HephAudio::AudioChannelMask::FrontLeft,
			{
				{ HephAudio::AudioChannelMask::FrontLeft, 1 },
				{ HephAudio::AudioChannelMask::FrontRight, 0 },
				{ HephAudio::AudioChannelMask::FrontCenter, 0.5 },
				{ HephAudio::AudioChannelMask::LowFrequency, 0.5 },
				{ HephAudio::AudioChannelMask::FrontLeftOfCenter, 1 },
				{ HephAudio::AudioChannelMask::FrontRightOfCenter, 0 },
				{ HephAudio::AudioChannelMask::SideLeft, 1 },
				{ HephAudio::AudioChannelMask::SideRight, 0 },
				{ HephAudio::AudioChannelMask::BackLeft, 1 },
				{ HephAudio::AudioChannelMask::BackRight, 0 },
				{ HephAudio::AudioChannelMask::BackCenter, 0.5 },
				{ HephAudio::AudioChannelMask::TopCenter, 0.5 },
				{ HephAudio::AudioChannelMask::TopFrontLeft, 1 },
				{ HephAudio::AudioChannelMask::TopFrontRight, 0 },
				{ HephAudio::AudioChannelMask::TopFrontCenter, 0.5 },
				{ HephAudio::AudioChannelMask::TopBackLeft, 1 },
				{ HephAudio::AudioChannelMask::TopBackRight, 0 },
				{ HephAudio::AudioChannelMask::TopBackCenter, 0.5 }
			}
		},
		{
			HephAudio::AudioChannelMask::FrontRight,
			{
				{ HephAudio::AudioChannelMask::FrontLeft, 0 },
				{ HephAudio::AudioChannelMask::FrontRight, 1 },
				{ HephAudio::AudioChannelMask::FrontCenter, 0.5 },
				{ HephAudio::AudioChannelMask::LowFrequency, 0.5 },
				{ HephAudio::AudioChannelMask::FrontLeftOfCenter, 0 },
				{ HephAudio::AudioChannelMask::FrontRightOfCenter, 1 },
				{ HephAudio::AudioChannelMask::SideLeft, 0 },
				{ HephAudio::AudioChannelMask::SideRight, 1 },
				{ HephAudio::AudioChannelMask::BackLeft, 0 },
				{ HephAudio::AudioChannelMask::BackRight, 1 },
				{ HephAudio::AudioChannelMask::BackCenter, 0.5 },
				{ HephAudio::AudioChannelMask::TopCenter, 0.5 },
				{ HephAudio::AudioChannelMask::TopFrontLeft, 0 },
				{ HephAudio::AudioChannelMask::TopFrontRight, 1 },
				{ HephAudio::AudioChannelMask::TopFrontCenter, 0.5 },
				{ HephAudio::AudioChannelMask::TopBackLeft, 0 },
				{ HephAudio::AudioChannelMask::TopBackRight, 1 },
				{ HephAudio::AudioChannelMask::TopBackCenter, 0.5 }
			}
		}
	};

	hephaudio_channel_mixing_lookup_table_t AudioChannelMixingLookupTables::_3_channels_table =
	{
		{
			HephAudio::AudioChannelMask::FrontLeft,
			{
				{ HephAudio::AudioChannelMask::FrontLeft, 1 },
				{ HephAudio::AudioChannelMask::FrontRight, 0 },
				{ HephAudio::AudioChannelMask::FrontCenter, 0 },
				{ HephAudio::AudioChannelMask::LowFrequency, 1 },
				{ HephAudio::AudioChannelMask::FrontLeftOfCenter, 0.5 },
				{ HephAudio::AudioChannelMask::FrontRightOfCenter, 0 },
				{ HephAudio::AudioChannelMask::SideLeft, 1 },
				{ HephAudio::AudioChannelMask::SideRight, 0 },
				{ HephAudio::AudioChannelMask::BackLeft, 1 },
				{ HephAudio::AudioChannelMask::BackRight, 0 },
				{ HephAudio::AudioChannelMask::BackCenter, 0 },
				{ HephAudio::AudioChannelMask::TopCenter, 0 },
				{ HephAudio::AudioChannelMask::TopFrontLeft, 1 },
				{ HephAudio::AudioChannelMask::TopFrontRight, 0 },
				{ HephAudio::AudioChannelMask::TopFrontCenter, 0 },
				{ HephAudio::AudioChannelMask::TopBackLeft, 1 },
				{ HephAudio::AudioChannelMask::TopBackRight, 0 },
				{ HephAudio::AudioChannelMask::TopBackCenter, 0 }
			}
		},
		{
			HephAudio::AudioChannelMask::FrontRight,
			{
				{ HephAudio::AudioChannelMask::FrontLeft, 0 },
				{ HephAudio::AudioChannelMask::FrontRight, 1 },
				{ HephAudio::AudioChannelMask::FrontCenter, 0 },
				{ HephAudio::AudioChannelMask::LowFrequency, 1 },
				{ HephAudio::AudioChannelMask::FrontLeftOfCenter, 0 },
				{ HephAudio::AudioChannelMask::FrontRightOfCenter, 0.5 },
				{ HephAudio::AudioChannelMask::SideLeft, 0 },
				{ HephAudio::AudioChannelMask::SideRight, 1 },
				{ HephAudio::AudioChannelMask::BackLeft, 0 },
				{ HephAudio::AudioChannelMask::BackRight, 1 },
				{ HephAudio::AudioChannelMask::BackCenter, 0 },
				{ HephAudio::AudioChannelMask::TopCenter, 0 },
				{ HephAudio::AudioChannelMask::TopFrontLeft, 0 },
				{ HephAudio::AudioChannelMask::TopFrontRight, 1 },
				{ HephAudio::AudioChannelMask::TopFrontCenter, 0 },
				{ HephAudio::AudioChannelMask::TopBackLeft, 0 },
				{ HephAudio::AudioChannelMask::TopBackRight, 1 },
				{ HephAudio::AudioChannelMask::TopBackCenter, 0 }
			}
		},
		{
			// for surround
			HephAudio::AudioChannelMask::FrontCenter,
			{
				{ HephAudio::AudioChannelMask::FrontLeft, 0 },
				{ HephAudio::AudioChannelMask::FrontRight, 0 },
				{ HephAudio::AudioChannelMask::FrontCenter, 1 },
				{ HephAudio::AudioChannelMask::LowFrequency, 0 },
				{ HephAudio::AudioChannelMask::FrontLeftOfCenter, 0.5 },
				{ HephAudio::AudioChannelMask::FrontRightOfCenter, 0.5 },
				{ HephAudio::AudioChannelMask::SideLeft, 0 },
				{ HephAudio::AudioChannelMask::SideRight, 0 },
				{ HephAudio::AudioChannelMask::BackLeft, 0 },
				{ HephAudio::AudioChannelMask::BackRight, 0 },
				{ HephAudio::AudioChannelMask::BackCenter, 1 },
				{ HephAudio::AudioChannelMask::TopCenter, 1 },
				{ HephAudio::AudioChannelMask::TopFrontLeft, 0 },
				{ HephAudio::AudioChannelMask::TopFrontRight, 0 },
				{ HephAudio::AudioChannelMask::TopFrontCenter, 1 },
				{ HephAudio::AudioChannelMask::TopBackLeft, 0 },
				{ HephAudio::AudioChannelMask::TopBackRight, 0 },
				{ HephAudio::AudioChannelMask::TopBackCenter, 1 }
			}
		},
		{
			// for 2_1
			HephAudio::AudioChannelMask::BackCenter,
			{
				{ HephAudio::AudioChannelMask::FrontLeft, 0 },
				{ HephAudio::AudioChannelMask::FrontRight, 0 },
				{ HephAudio::AudioChannelMask::FrontCenter, 1 },
				{ HephAudio::AudioChannelMask::LowFrequency, 0 },
				{ HephAudio::AudioChannelMask::FrontLeftOfCenter, 0.5 },
				{ HephAudio::AudioChannelMask::FrontRightOfCenter, 0.5 },
				{ HephAudio::AudioChannelMask::SideLeft, 0 },
				{ HephAudio::AudioChannelMask::SideRight, 0 },
				{ HephAudio::AudioChannelMask::BackLeft, 0 },
				{ HephAudio::AudioChannelMask::BackRight, 0 },
				{ HephAudio::AudioChannelMask::BackCenter, 1 },
				{ HephAudio::AudioChannelMask::TopCenter, 1 },
				{ HephAudio::AudioChannelMask::TopFrontLeft, 0 },
				{ HephAudio::AudioChannelMask::TopFrontRight, 0 },
				{ HephAudio::AudioChannelMask::TopFrontCenter, 1 },
				{ HephAudio::AudioChannelMask::TopBackLeft, 0 },
				{ HephAudio::AudioChannelMask::TopBackRight, 0 },
				{ HephAudio::AudioChannelMask::TopBackCenter, 1 }
			}
		},
		{
			// for 2_point_1
			HephAudio::AudioChannelMask::LowFrequency,
			{
				{ HephAudio::AudioChannelMask::FrontLeft, 0 },
				{ HephAudio::AudioChannelMask::FrontRight, 0 },
				{ HephAudio::AudioChannelMask::FrontCenter, 0 },
				{ HephAudio::AudioChannelMask::LowFrequency, 1 },
				{ HephAudio::AudioChannelMask::FrontLeftOfCenter, 0 },
				{ HephAudio::AudioChannelMask::FrontRightOfCenter, 0 },
				{ HephAudio::AudioChannelMask::SideLeft, 0 },
				{ HephAudio::AudioChannelMask::SideRight, 0 },
				{ HephAudio::AudioChannelMask::BackLeft, 0 },
				{ HephAudio::AudioChannelMask::BackRight, 0 },
				{ HephAudio::AudioChannelMask::BackCenter, 0 },
				{ HephAudio::AudioChannelMask::TopCenter, 0 },
				{ HephAudio::AudioChannelMask::TopFrontLeft, 0 },
				{ HephAudio::AudioChannelMask::TopFrontRight, 0 },
				{ HephAudio::AudioChannelMask::TopFrontCenter, 0 },
				{ HephAudio::AudioChannelMask::TopBackLeft, 0 },
				{ HephAudio::AudioChannelMask::TopBackRight, 0 },
				{ HephAudio::AudioChannelMask::TopBackCenter, 0 }
			}
		}
	};
}