#include "gtest/gtest.h"
#include "AudioFormatInfo.h"

using namespace Heph;
using namespace HephAudio;

TEST(AudioFormatInfoTest, Constructors)
{
	const uint16_t tag = HEPHAUDIO_FORMAT_TAG_MP3;
	const uint16_t bitsPerSample = 32;
	const AudioChannelLayout channelLayout = HEPHAUDIO_CH_LAYOUT_STEREO;
	const uint32_t sampleRate = 48000;
	const uint32_t bitRate = 320000;
	const Endian endian = Endian::Big;

	{
		const AudioFormatInfo format(tag, bitsPerSample, channelLayout, sampleRate);
		EXPECT_EQ(format.formatTag, tag);
		EXPECT_EQ(format.bitsPerSample, bitsPerSample);
		EXPECT_EQ(format.channelLayout, channelLayout);
		EXPECT_EQ(format.sampleRate, sampleRate);
		EXPECT_EQ(format.endian, HEPH_SYSTEM_ENDIAN);
	}

	{
		const AudioFormatInfo format(tag, bitsPerSample, channelLayout, sampleRate, bitRate);
		EXPECT_EQ(format.formatTag, tag);
		EXPECT_EQ(format.bitsPerSample, bitsPerSample);
		EXPECT_EQ(format.channelLayout, channelLayout);
		EXPECT_EQ(format.sampleRate, sampleRate);
		EXPECT_EQ(format.bitRate, bitRate);
		EXPECT_EQ(format.endian, HEPH_SYSTEM_ENDIAN);
	}

	{
		const AudioFormatInfo format(tag, bitsPerSample, channelLayout, sampleRate, endian);
		EXPECT_EQ(format.formatTag, tag);
		EXPECT_EQ(format.bitsPerSample, bitsPerSample);
		EXPECT_EQ(format.channelLayout, channelLayout);
		EXPECT_EQ(format.sampleRate, sampleRate);
		EXPECT_EQ(format.endian, endian);
	}

	{
		const AudioFormatInfo format(tag, bitsPerSample, channelLayout, sampleRate, bitRate, endian);
		EXPECT_EQ(format.formatTag, tag);
		EXPECT_EQ(format.bitsPerSample, bitsPerSample);
		EXPECT_EQ(format.channelLayout, channelLayout);
		EXPECT_EQ(format.sampleRate, sampleRate);
		EXPECT_EQ(format.bitRate, bitRate);
		EXPECT_EQ(format.endian, endian);
	}
}

TEST(AudioFormatInfoTest, Compare)
{
	const AudioFormatInfo s16(HEPHAUDIO_FORMAT_TAG_PCM, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const AudioFormatInfo s32(HEPHAUDIO_FORMAT_TAG_PCM, 32, HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const AudioFormatInfo f32_m(HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT, 32, HEPHAUDIO_CH_LAYOUT_MONO, 48000);
	const AudioFormatInfo f32_s(HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT, 32, HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const AudioFormatInfo f32_48k(HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT, 32, HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const AudioFormatInfo f32_96k(HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT, 32, HEPHAUDIO_CH_LAYOUT_STEREO, 96000);
	const AudioFormatInfo s16be(HEPHAUDIO_FORMAT_TAG_PCM, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000, Endian::Big);
	const AudioFormatInfo s16le(HEPHAUDIO_FORMAT_TAG_PCM, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000, Endian::Little);

	EXPECT_NE(s16, s32);
	EXPECT_NE(s32, f32_s);
	EXPECT_NE(f32_m, f32_s);
	EXPECT_NE(f32_48k, f32_96k);
	EXPECT_NE(s16be, s16le);

	EXPECT_EQ(s16, s16);
	EXPECT_EQ(s32, s32);
	EXPECT_EQ(f32_m, f32_m);
	EXPECT_EQ(f32_s, f32_s);
	EXPECT_EQ(f32_48k, f32_48k);
	EXPECT_EQ(f32_96k, f32_96k);
	EXPECT_EQ(s32, s32);
	EXPECT_EQ(s16be, s16be);
	EXPECT_EQ(s16le, s16le);
}

TEST(AudioFormatInfoTest, MemberMethods)
{
	AudioFormatInfo format(HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT, 32, HEPHAUDIO_CH_LAYOUT_7_POINT_1, 48000);

	{
		const size_t expectedFrameSize = (format.bitsPerSample / 8) * format.channelLayout.count;
		EXPECT_EQ(format.FrameSize(), expectedFrameSize);
	}
	
	{
		const size_t expectedByteRate = (format.bitsPerSample / 8) * format.channelLayout.count * format.sampleRate;
		EXPECT_EQ(format.ByteRate(), expectedByteRate);
	}

	{
		const size_t expectedBitRate = format.bitsPerSample * format.channelLayout.count * format.sampleRate;
		EXPECT_EQ(AudioFormatInfo::CalculateBitrate(format), expectedBitRate);
	}
}