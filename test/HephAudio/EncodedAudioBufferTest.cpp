#include "gtest/gtest.h"
#include "EncodedAudioBuffer.h"

using namespace Heph;
using namespace HephAudio;

TEST(EncodedAudioBufferTest, Constructors)
{
	const AudioFormatInfo expectedFormat = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t expectedSize = 512;

	{
		EncodedAudioBuffer b;
		EXPECT_EQ(b.Size(), 0);
		EXPECT_EQ(b.GetAudioFormatInfo(), AudioFormatInfo());
	}

	{
		EncodedAudioBuffer b(expectedFormat);
		EXPECT_EQ(b.Size(), 0);
		EXPECT_EQ(b.GetAudioFormatInfo(), expectedFormat);
	}

	{
		EncodedAudioBuffer b(expectedSize, expectedFormat);
		EXPECT_EQ(b.Size(), expectedSize);
		EXPECT_EQ(b.GetAudioFormatInfo(), expectedFormat);
	}

	{
		uint8_t data[expectedSize] = {};
		for (size_t i = 0; i < expectedSize; ++i)
			data[i] = i;

		EncodedAudioBuffer b(data, expectedSize, expectedFormat);
		EXPECT_EQ(b.Size(), expectedSize);
		EXPECT_EQ(b.GetAudioFormatInfo(), expectedFormat);

		for (size_t i = 0; i < expectedSize; ++i)
			EXPECT_EQ(b.begin()[i], data[i]);
	}
}

TEST(EncodedAudioBufferTest, Move)
{
	const AudioFormatInfo expectedFormat = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t expectedSize = 512;

	{
		EncodedAudioBuffer b1(expectedSize, expectedFormat);
		const uint8_t* expectedBegin = b1.begin();
		EncodedAudioBuffer b2(std::move(b1));

		EXPECT_TRUE(b1.begin() == nullptr);
		EXPECT_EQ(b1.Size(), 0);
		EXPECT_EQ(b1.GetAudioFormatInfo(), AudioFormatInfo());

		EXPECT_EQ(b2.begin(), expectedBegin);
		EXPECT_EQ(b2.Size(), expectedSize);
		EXPECT_EQ(b2.GetAudioFormatInfo(), expectedFormat);
	}

	{
		EncodedAudioBuffer b1(expectedSize, expectedFormat);
		EncodedAudioBuffer b2(128, AudioFormatInfo());

		const uint8_t* expectedBegin = b1.begin();
		b2 = std::move(b1);

		EXPECT_TRUE(b1.begin() == nullptr);
		EXPECT_EQ(b1.Size(), 0);
		EXPECT_EQ(b1.GetAudioFormatInfo(), AudioFormatInfo());

		EXPECT_EQ(b2.begin(), expectedBegin);
		EXPECT_EQ(b2.Size(), expectedSize);
		EXPECT_EQ(b2.GetAudioFormatInfo(), expectedFormat);
	}

	{
		EncodedAudioBuffer b(expectedSize, expectedFormat);
		const uint8_t* expectedBegin = b.begin();
		
		b = std::move(b);

		EXPECT_EQ(b.begin(), expectedBegin);
		EXPECT_EQ(b.Size(), expectedSize);
		EXPECT_EQ(b.GetAudioFormatInfo(), expectedFormat);
	}
}

TEST(EncodedAudioBufferTest, Release)
{
	const AudioFormatInfo expectedFormat = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t expectedSize = 512;

	EncodedAudioBuffer b(expectedSize, expectedFormat);
	b.Release();

	EXPECT_TRUE(b.begin() == nullptr);
	EXPECT_EQ(b.Size(), 0);
	EXPECT_EQ(b.GetAudioFormatInfo(), AudioFormatInfo());
}

TEST(EncodedAudioBufferTest, SetAudioFormat)
{
	const AudioFormatInfo expectedFormat = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t expectedSize = 512;

	EncodedAudioBuffer b(expectedSize, AudioFormatInfo());
	b.SetAudioFormatInfo(expectedFormat);
	EXPECT_EQ(b.GetAudioFormatInfo(), expectedFormat);
}

TEST(EncodedAudioBufferTest, Add)
{
	const AudioFormatInfo expectedFormat = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
	const size_t dataSize = 512;
	const size_t expectedSize = dataSize * 2;

	uint8_t data1[dataSize] = {};
	for (size_t i = 0; i < dataSize; ++i)
		data1[i] = i;

	uint8_t data2[dataSize] = {};
	for (size_t i = 0; i < dataSize; ++i)
		data2[i] = i + dataSize;

	EncodedAudioBuffer b1(data1, dataSize, expectedFormat);
	b1.Add(data2, dataSize);

	for (size_t i = 0; i < dataSize; ++i)
		EXPECT_EQ(b1.begin()[i], data1[i]);

	for (size_t i = 0; i < dataSize; ++i)
		EXPECT_EQ(b1.begin()[i + dataSize], data2[i]);
}