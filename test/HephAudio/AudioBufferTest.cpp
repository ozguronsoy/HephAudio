#include "gtest/gtest.h"
#include "AudioBuffer.h"

using namespace Heph;
using namespace HephAudio;

TEST(AudioBufferTest, Constructors)
{
	{
		AudioBuffer b;
		EXPECT_TRUE(b.begin() == nullptr);
		EXPECT_EQ(b.Size(), 0);
		EXPECT_EQ(b.FrameCount(), 0);
		EXPECT_EQ(b.FormatInfo(), AudioFormatInfo());
	}

	{
		const AudioFormatInfo formatInfo = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
		const size_t frameCount = 512;
		const size_t size = frameCount * formatInfo.channelLayout.count;

		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		EXPECT_TRUE(b1.begin() != nullptr);
		EXPECT_EQ(b1.Size(), size);
		EXPECT_EQ(b1.FrameCount(), frameCount);
		EXPECT_EQ(b1.FormatInfo(), formatInfo);

		AudioBuffer b2(frameCount, formatInfo.channelLayout, formatInfo.sampleRate, BufferFlags::AllocUninitialized);
		EXPECT_TRUE(b2.begin() != nullptr);
		EXPECT_EQ(b2.Size(), size);
		EXPECT_EQ(b2.FrameCount(), frameCount);
		EXPECT_EQ(b2.FormatInfo(), formatInfo);
	}
}

TEST(AudioBufferTest, Copy)
{
	const AudioFormatInfo formatInfo = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t frameCount = 512;
	const size_t size = frameCount * formatInfo.channelLayout.count;

	auto populate = [](AudioBuffer& b)
		{
			for (size_t i = 0, val = 0; i < b.FrameCount(); ++i)
				for (size_t j = 0; j < b.FormatInfo().channelLayout.count; ++j, ++val)
					b[i][j] = val;

		};

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		populate(b1);
		AudioBuffer b2(b1);

		EXPECT_EQ(b1, b2);
	}

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		populate(b1);
		AudioBuffer b2;

		b2.operator=(b1);

		EXPECT_EQ(b1, b2);
	}

	{
		AudioBuffer b(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		const heph_audio_sample_t* expectedBegin = b.begin();

		b = b;

		EXPECT_EQ(b.begin(), expectedBegin);
		EXPECT_EQ(b.Size(), size);
		EXPECT_EQ(b.FrameCount(), frameCount);
		EXPECT_EQ(b.FormatInfo(), formatInfo);
	}
}

TEST(AudioBufferTest, Move)
{
	const AudioFormatInfo formatInfo = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t frameCount = 512;
	const size_t size = frameCount * formatInfo.channelLayout.count;

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		const heph_audio_sample_t* expectedBegin = b1.begin();
		AudioBuffer b2(std::move(b1));

		EXPECT_TRUE(b1.begin() == nullptr);
		EXPECT_EQ(b1.Size(), 0);
		EXPECT_EQ(b1.FrameCount(), 0);
		EXPECT_EQ(b1.FormatInfo(), AudioFormatInfo());

		EXPECT_EQ(b2.begin(), expectedBegin);
		EXPECT_EQ(b2.Size(), size);
		EXPECT_EQ(b2.FrameCount(), frameCount);
		EXPECT_EQ(b2.FormatInfo(), formatInfo);
	}

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b2;
		const heph_audio_sample_t* expectedBegin = b1.begin();

		b2.operator=(std::move(b1));

		EXPECT_TRUE(b1.begin() == nullptr);
		EXPECT_EQ(b1.Size(), 0);
		EXPECT_EQ(b1.FrameCount(), 0);
		EXPECT_EQ(b1.FormatInfo(), AudioFormatInfo());

		EXPECT_EQ(b2.begin(), expectedBegin);
		EXPECT_EQ(b2.Size(), size);
		EXPECT_EQ(b2.FrameCount(), frameCount);
		EXPECT_EQ(b2.FormatInfo(), formatInfo);
	}

	{
		AudioBuffer b(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		const heph_audio_sample_t* expectedBegin = b.begin();

		b = std::move(b);

		EXPECT_EQ(b.begin(), expectedBegin);
		EXPECT_EQ(b.Size(), size);
		EXPECT_EQ(b.FrameCount(), frameCount);
		EXPECT_EQ(b.FormatInfo(), formatInfo);
	}
}

TEST(AudioBufferTest, ShiftOperators)
{
	const AudioFormatInfo formatInfo = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

	{
		AudioBuffer b1(3, formatInfo.channelLayout, formatInfo.sampleRate);
		b1[0][0] = 1;
		b1[0][1] = 2;
		b1[1][0] = 3;
		b1[1][1] = 4;
		b1[2][0] = 5;
		b1[2][1] = 6;
		AudioBuffer b2 = b1 << 2;

		EXPECT_EQ(b2.Size(), 6);
		EXPECT_EQ(b2.FrameCount(), 3);
		EXPECT_EQ(b2.FormatInfo(), formatInfo);

		const heph_audio_sample_t expected[3][2] = { {5, 6}, {0, 0}, {0, 0} };

		for (size_t i = 0; i < b2.FrameCount(); ++i)
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				EXPECT_EQ(b2[i][j], expected[i][j]);
	}

	{
		AudioBuffer b(3, formatInfo.channelLayout, formatInfo.sampleRate);
		b[0][0] = 1;
		b[0][1] = 2;
		b[1][0] = 3;
		b[1][1] = 4;
		b[2][0] = 5;
		b[2][1] = 6;

		const heph_audio_sample_t expected[3][2] = { {5, 6}, {0, 0}, {0, 0} };

		b <<= 2;

		EXPECT_EQ(b.Size(), 6);
		EXPECT_EQ(b.FrameCount(), 3);
		EXPECT_EQ(b.FormatInfo(), formatInfo);

		for (size_t i = 0; i < b.FrameCount(); ++i)
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				EXPECT_EQ(b[i][j], expected[i][j]);
	}

	{
		AudioBuffer b1(3, formatInfo.channelLayout, formatInfo.sampleRate);
		b1[0][0] = 1;
		b1[0][1] = 2;
		b1[1][0] = 3;
		b1[1][1] = 4;
		b1[2][0] = 5;
		b1[2][1] = 6;
		AudioBuffer b2 = b1 >> 2;

		EXPECT_EQ(b2.Size(), 6);
		EXPECT_EQ(b2.FrameCount(), 3);
		EXPECT_EQ(b2.FormatInfo(), formatInfo);

		const heph_audio_sample_t expected[3][2] = { {0, 0}, {0, 0}, {1, 2} };

		for (size_t i = 0; i < b2.FrameCount(); ++i)
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				EXPECT_EQ(b2[i][j], expected[i][j]);
	}

	{
		AudioBuffer b(3, formatInfo.channelLayout, formatInfo.sampleRate);
		b[0][0] = 1;
		b[0][1] = 2;
		b[1][0] = 3;
		b[1][1] = 4;
		b[2][0] = 5;
		b[2][1] = 6;

		const heph_audio_sample_t expected[3][2] = { {0, 0}, {0, 0}, {1, 2} };

		b >>= 2;

		EXPECT_EQ(b.Size(), 6);
		EXPECT_EQ(b.FrameCount(), 3);
		EXPECT_EQ(b.FormatInfo(), formatInfo);

		for (size_t i = 0; i < b.FrameCount(); ++i)
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
				EXPECT_EQ(b[i][j], expected[i][j]);
	}
}

TEST(AudioBufferTest, Compare)
{
	const AudioFormatInfo formatInfo = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t frameCount = 512;

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b2 = b1;
		AudioBuffer b3(0, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b4(frameCount, HEPHAUDIO_CH_LAYOUT_MONO, formatInfo.sampleRate);
		AudioBuffer b5(frameCount, formatInfo.channelLayout, formatInfo.sampleRate * 2);

		EXPECT_EQ(b1, b2);
		EXPECT_NE(b1, b3);
		EXPECT_NE(b1, b4);
		EXPECT_NE(b1, b5);
	}

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b2 = b1;
		AudioBuffer b3 = b1;

		for (size_t i = 0; i < frameCount; ++i)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				b1[i][j] = (i * formatInfo.channelLayout.count) + j;
				b2[i][j] = b1[i][j];
				b3[i][j] = b1[i][j] + 5;
			}
		}

		EXPECT_EQ(b1, b2);
		EXPECT_NE(b1, b3);
	}
}

TEST(AudioBufferTest, OtherMethods)
{
	const AudioFormatInfo formatInfo = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t frameCount = 512;

	{
		AudioBuffer b(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer subBuffer = b.SubBuffer(128, 256);

		EXPECT_EQ(subBuffer.Size(), 256 * formatInfo.channelLayout.count);
		EXPECT_EQ(subBuffer.FrameCount(), 256);
		EXPECT_EQ(subBuffer.FormatInfo(), formatInfo);
	}

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b2(frameCount / 2, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b3(frameCount / 2, formatInfo.channelLayout, formatInfo.sampleRate * 2);
		AudioBuffer b4(frameCount / 2, HEPHAUDIO_CH_LAYOUT_MONO, formatInfo.sampleRate);

		b1.Prepend(b2);
		EXPECT_EQ(b1.Size(), (frameCount + frameCount / 2) * formatInfo.channelLayout.count);
		EXPECT_EQ(b1.FrameCount(), (frameCount + frameCount / 2));
		EXPECT_EQ(b1.FormatInfo(), formatInfo);

		EXPECT_THROW(b1.Prepend(b3), InvalidArgumentException);
		EXPECT_THROW(b1.Prepend(b4), InvalidArgumentException);
	}

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b2(frameCount / 2, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b3(frameCount / 2, formatInfo.channelLayout, formatInfo.sampleRate * 2);
		AudioBuffer b4(frameCount / 2, HEPHAUDIO_CH_LAYOUT_MONO, formatInfo.sampleRate);

		b1.Append(b2);
		EXPECT_EQ(b1.Size(), (frameCount + frameCount / 2) * formatInfo.channelLayout.count);
		EXPECT_EQ(b1.FrameCount(), (frameCount + frameCount / 2));
		EXPECT_EQ(b1.FormatInfo(), formatInfo);

		EXPECT_THROW(b1.Append(b3), InvalidArgumentException);
		EXPECT_THROW(b1.Append(b4), InvalidArgumentException);
	}

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b2(frameCount / 2, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b3(frameCount / 2, formatInfo.channelLayout, formatInfo.sampleRate * 2);
		AudioBuffer b4(frameCount / 2, HEPHAUDIO_CH_LAYOUT_MONO, formatInfo.sampleRate);

		b1.Insert(b2, b1.FrameCount() / 2);
		EXPECT_EQ(b1.Size(), (frameCount + frameCount / 2) * formatInfo.channelLayout.count);
		EXPECT_EQ(b1.FrameCount(), (frameCount + frameCount / 2));
		EXPECT_EQ(b1.FormatInfo(), formatInfo);

		EXPECT_THROW(b1.Insert(b3, b1.FrameCount() / 2), InvalidArgumentException);
		EXPECT_THROW(b1.Insert(b4, b1.FrameCount() / 2), InvalidArgumentException);
	}

	{
		AudioBuffer b(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);

		b.Cut(0, frameCount / 2);
		EXPECT_EQ(b.Size(), (frameCount / 2) * formatInfo.channelLayout.count);
		EXPECT_EQ(b.FrameCount(), frameCount / 2);
		EXPECT_EQ(b.FormatInfo(), formatInfo);
	}

	{
		AudioBuffer b1(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b2(frameCount / 2, formatInfo.channelLayout, formatInfo.sampleRate);
		AudioBuffer b3(frameCount / 2, formatInfo.channelLayout, formatInfo.sampleRate * 2);
		AudioBuffer b4(frameCount / 2, HEPHAUDIO_CH_LAYOUT_MONO, formatInfo.sampleRate);

		b1.Replace(b2, frameCount / 2, frameCount / 2);
		EXPECT_EQ(b1.Size(), frameCount * formatInfo.channelLayout.count);
		EXPECT_EQ(b1.FrameCount(), frameCount);
		EXPECT_EQ(b1.FormatInfo(), formatInfo);

		EXPECT_THROW(b1.Replace(b3, frameCount / 2, frameCount / 2), InvalidArgumentException);
		EXPECT_THROW(b1.Replace(b4, frameCount / 2, frameCount / 2), InvalidArgumentException);
	}

	{
		AudioBuffer b(frameCount, formatInfo.channelLayout, formatInfo.sampleRate);

		b.Resize(frameCount);
		EXPECT_EQ(b.Size(), frameCount * formatInfo.channelLayout.count);
		EXPECT_EQ(b.FrameCount(), frameCount);
		EXPECT_EQ(b.FormatInfo(), formatInfo);

		b.Resize(frameCount / 2);
		EXPECT_EQ(b.Size(), (frameCount / 2) * formatInfo.channelLayout.count);
		EXPECT_EQ(b.FrameCount(), frameCount / 2);
		EXPECT_EQ(b.FormatInfo(), formatInfo);

		b.Resize(frameCount);
		EXPECT_EQ(b.Size(), frameCount * formatInfo.channelLayout.count);
		EXPECT_EQ(b.FrameCount(), frameCount);
		EXPECT_EQ(b.FormatInfo(), formatInfo);
	}

	{
		AudioBuffer b(3, formatInfo.channelLayout, formatInfo.sampleRate);
		b[0][0] = 1;
		b[0][1] = 2;
		b[1][0] = 3;
		b[1][1] = 4;
		b[2][0] = 5;
		b[2][1] = 6;

		AudioBuffer br(3, formatInfo.channelLayout, formatInfo.sampleRate);
		br[2][0] = 1;
		br[2][1] = 2;
		br[1][0] = 3;
		br[1][1] = 4;
		br[0][0] = 5;
		br[0][1] = 6;

		b.Reverse();
		EXPECT_EQ(b, br);
	}
}

TEST(AudioBufferTest, SetFormat)
{
	AudioBuffer b(512, HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

	b.SetSampleRate(96000);
	EXPECT_EQ(b.Size(), 512 * 2);
	EXPECT_EQ(b.FrameCount(), 512);
	EXPECT_EQ(b.FormatInfo().channelLayout, HEPHAUDIO_CH_LAYOUT_STEREO);
	EXPECT_EQ(b.FormatInfo().sampleRate, 96000);

	b.SetChannelLayout(HEPHAUDIO_CH_LAYOUT_7_POINT_1);
	EXPECT_EQ(b.Size(), 512 * 8);
	EXPECT_EQ(b.FrameCount(), 512);
	EXPECT_EQ(b.FormatInfo().channelLayout, HEPHAUDIO_CH_LAYOUT_7_POINT_1);
	EXPECT_EQ(b.FormatInfo().sampleRate, 96000);
}