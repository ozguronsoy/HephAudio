#include "gtest/gtest.h"
#include "AudioChannelLayout.h"

using namespace Heph;
using namespace HephAudio;

TEST(AudioChannelLayoutTest, MaskOperators)
{
	const uint32_t expectedStereo = ((uint32_t)AudioChannelMask::FrontLeft) | ((uint32_t)AudioChannelMask::FrontRight);
	const uint32_t expected2point1 = expectedStereo | ((uint32_t)AudioChannelMask::LowFrequency);

	AudioChannelMask mask = AudioChannelMask::FrontLeft | AudioChannelMask::FrontRight;
	EXPECT_EQ((uint32_t)mask, expectedStereo);

	mask |= AudioChannelMask::LowFrequency;
	EXPECT_EQ(mask, expected2point1);

	EXPECT_EQ(mask & AudioChannelMask::LowFrequency, AudioChannelMask::LowFrequency);
	mask &= AudioChannelMask::FrontRight;
	EXPECT_EQ(mask, AudioChannelMask::FrontRight);
}

TEST(AudioChannelLayoutTest, Constructors)
{
	{
		const AudioChannelLayout empty;
		EXPECT_EQ(empty.count, 0);
		EXPECT_EQ(empty.mask, AudioChannelMask::Unknown);
	}

	{
		const AudioChannelLayout stereo(2, HEPHAUDIO_CH_MASK_STEREO);
		EXPECT_EQ(stereo.count, 2);
		EXPECT_EQ(stereo.mask, HEPHAUDIO_CH_MASK_STEREO);
	}
}

TEST(AudioChannelLayoutTest, Compare)
{
	{
		const AudioChannelLayout stereo = HEPHAUDIO_CH_LAYOUT_STEREO;

		EXPECT_EQ(stereo, HEPHAUDIO_CH_LAYOUT_STEREO);
		EXPECT_NE(stereo, AudioChannelLayout(1, HEPHAUDIO_CH_MASK_STEREO));
		EXPECT_NE(stereo, AudioChannelLayout(2, HEPHAUDIO_CH_MASK_7_POINT_1));
	}
}

TEST(AudioChannelLayoutTest, GetChannelCount)
{
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_MASK_MONO), 1);
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_MASK_STEREO), 2);
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_MASK_4_POINT_0), 4);
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_MASK_5_POINT_1), 6);
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_MASK_7_POINT_1), 8);

	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_LAYOUT_MONO), 1);
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_LAYOUT_STEREO), 2);
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_LAYOUT_4_POINT_0), 4);
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_LAYOUT_5_POINT_1), 6);
	EXPECT_EQ(AudioChannelLayout::GetChannelCount(HEPHAUDIO_CH_LAYOUT_7_POINT_1), 8);
}