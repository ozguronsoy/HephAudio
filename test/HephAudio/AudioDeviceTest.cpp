#include "gtest/gtest.h"
#include "AudioDevice.h"

using namespace Heph;
using namespace HephAudio;

TEST(AudioDeviceTest, AudioDeviceType)
{
	const uint32_t all = ((uint32_t)AudioDeviceType::Render) | ((uint32_t)AudioDeviceType::Capture);

	AudioDeviceType type = AudioDeviceType::All;
	EXPECT_EQ((uint32_t)type, all);

	type = AudioDeviceType::Render;
	type |= AudioDeviceType::Capture;
	EXPECT_EQ((uint32_t)type, all);

	EXPECT_EQ(type & AudioDeviceType::Capture, AudioDeviceType::Capture);
	type &= AudioDeviceType::Render;
	EXPECT_EQ(type, AudioDeviceType::Render);
}